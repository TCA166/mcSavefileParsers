#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <math.h>

#include "errorDefs.h"
#include "regionParser.h"

//Program for extracting specific chunks from all region files
int main(int argc, char** argv){
    if(argc < 3){
        argCountError();
    }
    for(int i = 3; i < argc; i+=3){
        int x = atoi(argv[i - 1]);
        int z = atoi(argv[i]);
        chunk ourChunk = extractChunk(argv[i - 2], x, z);
        char* filename = malloc(8 + 10 + 10);
        sprintf(filename, "./%d.%d.nbt", x, z);
        FILE* outFile = fopen(filename, "wb");
        free(filename);
        fwrite(ourChunk.data, ourChunk.byteLength, 1, outFile);
        fclose(outFile);
        free(ourChunk.data);
    }
}   