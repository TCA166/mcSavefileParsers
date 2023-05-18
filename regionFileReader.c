#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>
#include <string.h>

//https://minecraft.fandom.com/wiki/Region_file_format

//How many chunks are in a region
#define chunkN 1024

//Length in bytes of savefile segment 
#define segmentLength 4096

//How to translate the compression byte
#define GZip 1
#define Zlib 2
#define Uncompressed 3

#define getRegion(coord) coord>>5

#define coordsToOffset(x, z) 4 * ((x & 31) + (z & 31) * 32)

//Unsigned char
typedef unsigned char byte;

//A struct representing a mc game chunk
struct chunk{
    int x; //x coordinate in chunk coordinates
    int z; //z coordinate in chunk coordinates
    unsigned int offset; //3 bytes of offset
    byte sectorCount; //1 byte indicating chunk data length
    unsigned int timestamp;
    unsigned int byteLength;
    byte compression;
    byte* data; //pointer to decompressed bytes
};

//struct chunk
typedef struct chunk chunk;

/*
Extracts all the chunks in regionFile.
Returns a dynamic array of chunks with 1024 chunks.
*/
chunk* getChunks(FILE* regionFile){
    chunk* chunks = malloc(sizeof(chunk) * chunkN);
    fseek(regionFile, 0, SEEK_SET);
    //despite what it might seem with these loops we are doing a simple linear go through the file
    //first there is a 4096 byte long section of 1024 4 bit fields. Each field is made up of 3 big endian encoded int24s and a single int8
    for(int i = 0; i < chunkN; i++){
        chunk newChunk;
        //so the numbers are stored as big endian AND as int24
        byte bytes[3];
        fread(&bytes, 1, 3, regionFile);
        newChunk.offset = (bytes[0] << 16) + (bytes[1] << 8) + bytes[2];
        //Luckily reading a single byte is simpler
        fread(&newChunk.sectorCount, 1, 1, regionFile);
        chunks[i] = newChunk;
    }
    //Then there's an equally long section made up of 1024 int32 timestamps
    for(int i = 0; i < chunkN; i++){
        byte bytes[4];
        fread(&bytes, 1, 4, regionFile);
        chunks[i].timestamp = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    }
    //Then there's encoded chunk data in n*4096 byte long chunks
    //Each of these chunks is made up of a single int32 field that contains the length of the preceding compressed data
    //foreach chunk we have extracted so far
    for(int i = 0; i < chunkN; i++){
        if(chunks[i].offset != 0){ //if the chunk isn't NULL
            fseek(regionFile, segmentLength * chunks[i].offset, SEEK_SET); //find the corresponding section
            //get the byteLength
            byte bytes[4];
            fread(&bytes, 1, 4, regionFile);
            chunks[i].byteLength = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
            //get the compression type
            fread(&chunks[i].compression, 1, 1, regionFile);
            //fseek(regionFile, 1, SEEK_CUR); // roll back the pointer since the byteLength includes that one byte
            //Then get the data
            chunks[i].byteLength += 5;
            byte* data = malloc(chunks[i].byteLength);
            fread(data, 1, chunks[i].byteLength, regionFile);
            //fseek(regionFile, (chunks[i].sectorCount * segmentLength) - chunks[i].byteLength, SEEK_CUR);
            if(chunks[i].compression == Uncompressed){
                chunks[i].data = data;
            }
            else{
                uLongf buffSize = segmentLength;
                byte* buff = malloc(buffSize);
                int res;
                //decompress the data
                if(chunks[i].compression == Zlib){
                    do{
                        buffSize += segmentLength;
                        buff = realloc(buff, buffSize);
                        res = uncompress((Bytef *)buff, &buffSize, data, chunks[i].byteLength);
                    }
                    while(res == -5);
                }
                else{
                    uLong sourceLen = (uLong)chunks[i].byteLength;
                    do{
                        buffSize += segmentLength;
                        buff = realloc(buff, buffSize);
                        res = uncompress2((Bytef *)buff, &buffSize, data, &sourceLen);
                    }
                    while(res == -5);
                }
                
                chunks[i].byteLength = buffSize;
                buff = realloc(buff, buffSize);
                if(res < 0){
                    fprintf(stderr, "Inflate returned %d ", res);
                    perror("Decompression failed.");
                }
                chunks[i].data = buff;
                free(data);
            }
        }
    }
    return chunks;
}

int main(int argc, char** argv){
    //foreach argument
    for(int i = 2; i < argc; i+=2){
        char* filename = argv[i - 1];
        FILE* regionFile = fopen(filename, "r");
        chunk* chunks = getChunks(regionFile);
        fclose(regionFile);
        for(int n = 0; n < chunkN; n++){
            if(chunks[n].offset != 0){
                char* filename = malloc(10 + strlen(argv[i]));
                sprintf(filename, "%s/%d.nbt", argv[i], chunks[n].offset);
                FILE* chunkFile = fopen(filename, "w");
                free(filename);
                fwrite(chunks[n].data, chunks[n].byteLength, 1, chunkFile);
                fclose(chunkFile);
            }   
            
        }
        
        free(chunks);
    }
}