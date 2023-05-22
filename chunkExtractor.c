#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "regionParser.h"

int digits(int i){
    if(i == 0){
        return 1;
    }
    return floor(log10(abs(i))) + 1;
}

//Program for extracting specific chunks from all region files
int main(int argc, char** argv){
    for(int i = 3; i < argc; i+=3){
        int x = atoi(argv[i - 1]);
        int z = atoi(argv[i]);
        char* filename = malloc(strlen(argv[i - 2]) + digits(x) + digits(z) + 9);
        sprintf(filename, "%s/r.%d.%d.mca", argv[i - 2], getRegion(x), getRegion(z));
        FILE* regionFile = fopen(filename, "r");
        if(regionFile == NULL){ \
            fprintf(stderr, "File %s couldn't be located.", filename); \
            return -1; \
        }
        free(filename);
        chunk ourChunk = getChunk(x, z, regionFile);
        fclose(regionFile);
        filename = malloc(8 + digits(x) + digits(z));
        sprintf(filename, "./%d.%d.nbt", x, z);
        FILE* outFile = fopen(filename, "w");
        free(filename);
        fwrite(ourChunk.data, ourChunk.byteLength, 1, outFile);
        fclose(outFile);
        free(ourChunk.data);
    }
}   