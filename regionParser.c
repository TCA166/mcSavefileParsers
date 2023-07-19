#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <math.h>
#include <zlib.h>

#include "regionParser.h"
#include "errorDefs.h"

#define getRegion(coord) coord>>5

#define coordsToOffset(x, z) 4 * ((x & 31) + (z & 31) * 32)

int handleFirstSegment(chunk* output, FILE* regionFile, char* regionFileName){
    //so the numbers are stored as big endian AND as int24
    byte bytes[3];
    size_t s1 = fread(&bytes, 1, 3, regionFile);
    output->offset = (bytes[0] << 16) + (bytes[1] << 8) + bytes[2];
    //do an offset check
    long pos = ftell(regionFile);
    fseek(regionFile, 0, SEEK_END);
    if(output->offset > ftell(regionFile)){
        output->offset = -1;
        return 1;
    }
    fseek(regionFile, pos, SEEK_SET);
    //Luckily reading a single byte is simpler
    size_t s2 = fread(&output->sectorCount, 1, 1, regionFile);
    //0 if everything is fine
    return (s1 + s2 == 4) - 1 || (output->offset == 0 && output->sectorCount == 0);
}

int handleSecondSegment(chunk* output, FILE* regionFile){
    byte bytes[4];
    size_t s1 = fread(&bytes, 1, 4, regionFile);
    output->timestamp = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    return (s1 == 4) - 1 && output->timestamp > 0;
}

int getChunkData(chunk* thisChunk, FILE* regionFile, char* regionFileName){
    //find the corresponding section
    if(fseek(regionFile, segmentLength * thisChunk->offset, SEEK_SET) != 0){
        fileError(regionFileName, "seek");
    } 
    //get the byteLength
    byte bytes[4];
    if(fread(&bytes, 1, 4, regionFile) != 4){
        localizedFileError((unsigned long int)(segmentLength * thisChunk->offset), regionFileName, "parsed:1");
    }
    thisChunk->byteLength = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    //get the compression type
    if(fread(&thisChunk->compression, 1, 1, regionFile) != 1){
        fileError(regionFileName, "parsed:2");
    }
    //fseek(regionFile, 2, SEEK_CUR);
    //Then get the data
    thisChunk->byteLength += 5;
    byte* data = malloc(thisChunk->byteLength);
    if(fread(data, 1, thisChunk->byteLength, regionFile) != thisChunk->byteLength){
        fileError(regionFileName, "parsed:3");
    }
    //fseek(regionFile, (chunks[i].sectorCount * segmentLength) - chunks[i].byteLength, SEEK_CUR);
    //handle different compression types
    if(thisChunk->compression == Uncompressed){
        thisChunk->data = data;
    }
    else{
        uLongf buffSize = segmentLength;
        byte* buff = malloc(buffSize);
        int res;
        //decompress the data
        if(thisChunk->compression == Zlib){
            do{
                buffSize += segmentLength;
                buff = realloc(buff, buffSize);
                res = uncompress((Bytef *)buff, &buffSize, data, thisChunk->byteLength);
            }
            while(res == -5);
        }
        else{
            uLong sourceLen = (uLong)thisChunk->byteLength;
            do{
                buffSize += segmentLength;
                buff = realloc(buff, buffSize);
                res = uncompress2((Bytef *)buff, &buffSize, data, &sourceLen);
            }
            while(res == -5);
        }
                
        thisChunk->byteLength = buffSize;
        buff = realloc(buff, buffSize);
        if(res < 0){
            return res;
        }
        thisChunk->data = buff;
        free(data);
    }
    return 0;
}

chunk* getChunks(FILE* regionFile){
    chunk* chunks = malloc(sizeof(chunk) * chunkN);
    fseek(regionFile, 0, SEEK_SET);
    //despite what it might seem with these loops we are doing a simple linear go through the file
    //first there is a 4096 byte long section of 1024 4 bit fields. Each field is made up of 3 big endian encoded int24s and a single int8
    for(int i = 0; i < chunkN; i++){
        chunk newChunk;
        if(handleFirstSegment(&newChunk, regionFile, "region file") != 0){
            parsingError("region file", "first segment");
        }
        chunks[i] = newChunk;
    }
    //Then there's an equally long section made up of 1024 int32 timestamps
    for(int i = 0; i < chunkN; i++){
        if(handleSecondSegment(&chunks[i], regionFile) != 0){
            parsingError("region file", "second segment");
        }
    }
    //Then there's encoded chunk data in n*4096 byte long chunks
    //Each of these chunks is made up of a single int32 field that contains the length of the preceding compressed data
    //foreach chunk we have extracted so far
    for(int i = 0; i < chunkN; i++){
        if(chunks[i].offset != 0){ //if the chunk isn't NULL
            /*
            At one point i did in fact attempt to move this function to linear file parsing instead of the jumping thing we have now
            Two major issues.
            One: less stability. A single error that may be caused by corrupted data throws the entire algorythm off which is exactly what happened during testing
            Two: Sometimes the header data about a chunk would be straight up wrong? chunkLen would be greater than the suggested cap, or fill less than 1000 bytes but have allocated three segments
            All of this chicanery made me simply give up and opt for this clearly more stable and safer option
            */
            int res = getChunkData(&chunks[i], regionFile, "region file");
            if(res < 0){
                fprintf(stderr, "Inflate returned %d ", res);
                perror("Decompression failed.");
            }
        }
    }
    return chunks;
}

chunk getChunk(int x, int z, FILE* regionFile, char* regionFileName){
    int offset = coordsToOffset(x, z);
    chunk result;
    result.x = x;
    result.z = z;
    if(fseek(regionFile, offset, SEEK_SET) != 0){
        fileError("region file", "seek");
    }
    if(handleFirstSegment(&result, regionFile, regionFileName) != 0){
        return result;
    }
    if(fseek(regionFile, offset + segmentLength, SEEK_SET) != 0){
        fileError("region file", "seek");
    }
    if(handleSecondSegment(&result, regionFile) != 0){
        return result;
    }
    if(result.offset == 0){
        return result;
    }
    int res = getChunkData(&result, regionFile, regionFileName);
    if(res < 0){
        fprintf(stderr, "Inflate returned %d ", res);
        perror("Decompression failed.");
    }
    return result;
}

chunk extractChunk(char* regionDirPath, int x, int z){
    char* filename = malloc(strlen(regionDirPath) + 10 + 10 + 9);
    sprintf(filename, "%s/r.%d.%d.mca", regionDirPath, getRegion(x), getRegion(z));
    FILE* regionFile = fopen(filename, "rb");
    if(regionFile == NULL){
        fileError(filename, "located");
    }
    chunk ourChunk = getChunk(x, z, regionFile, filename);
    //I could use errno here, or rework getChunk so that it returns an errno value, but I like it this way better
    if(!ourChunk.offset && !ourChunk.sectorCount){
        parsingError(filename, "parsing of the first segment; chunk isn't saved")
    }
    else if(ourChunk.offset == -1){
        parsingError(filename, "parsing of the first segment; chunk offset is corrupted and larger than filesize")
    }
    else if(ourChunk.timestamp < 0){
        parsingError(filename, "parsing of the second segment")
    }
    free(filename);
    fclose(regionFile);
    return ourChunk;
}