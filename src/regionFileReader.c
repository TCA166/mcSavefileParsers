/*!
 @file chunkExtractor.c
 @brief Extracts all chunks from all region files
 @details This program extracts all chunks from all region files
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "./lib/errorDefs.h"
#include "./lib/regionParser.h"

//https://minecraft.fandom.com/wiki/Region_file_format

/*!
 @brief Extracts all chunks from all region files
 @details Loads all chunks from all region files using the regionParser library, then writes them to files in the given directories.
 @see regionParser.h
 @see extractChunk
*/
int main(int argc, char** argv){
    if(argc < 2){
        argCountError("regionFileReader <path to region file> <output directory>");
        raiseError("argCountError");
    }
    //foreach argument
    for(int i = 2; i < argc; i+=2){
        printf("Starting extraction of %s to %s\n", argv[i - 1], argv[i]);
        char* filename = argv[i - 1];
        FILE* regionFile = fopen(filename, "rb");
        if(regionFile == NULL){ 
            fileError(argv[i - 1], "opened");
        }
        chunk* chunks = getChunks(regionFile);
        fclose(regionFile);
        for(int n = 0; n < chunkN; n++){
            if(chunks[n].offset != 0){
                char* filename = malloc(10 + strlen(argv[i]));
                sprintf(filename, "%s/%d.nbt", argv[i], chunks[n].offset);
                FILE* chunkFile = fopen(filename, "wb");
                if(chunkFile == NULL){
                    fileError(filename, "opened");
                    raiseError("fopen");
                }
                if(fwrite(chunks[n].data, chunks[n].byteLength, 1, chunkFile) != 1){
                    fileError(filename, "written to");
                    raiseError("fwrite");
                }
                free(filename);
                free(chunks[n].data);
                fclose(chunkFile);
            }   
        }
        free(chunks);
        printf("Done\n");
    }
    return 0;
}