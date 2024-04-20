/*!
 @file chunkExtractor.c
 @brief Extracts specific chunks from all region files
 @details This program extracts specific chunks from all region files   
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <math.h>

#include "./lib/errorDefs.h"
#include "./lib/regionParser.h"

/*!
 @brief Extracts a chunk from a region file
 @details Loads a chunk from a region file using the regionParser library, then writes it to a file using extractChunk.
 @see regionParser.h
 @see @ref extractChunk
*/
int main(int argc, char** argv){
    if(argc < 3){
        argCountError("chunkExtractor <path to region directory> <x> <z>");
        errorCheck("argCountError");
    }
    for(int i = 3; i < argc; i+=3){
        int x = atoi(argv[i - 1]);
        int z = atoi(argv[i]);
        chunk ourChunk = extractChunk(argv[i - 2], x, z);
        errorCheck("extractChunk");
        char* filename = malloc(8 + 10 + 10);
        sprintf(filename, "./%d.%d.nbt", x, z);
        FILE* outFile = fopen(filename, "wb");
        if(outFile == NULL){
            errorCheck("fopen");
        }
        if(fwrite(ourChunk.data, ourChunk.byteLength, 1, outFile) != 1){
            errorCheck("fwrite");
        }
        free(filename);
        fclose(outFile);
        free(ourChunk.data);
    }
    return EXIT_SUCCESS;
}   