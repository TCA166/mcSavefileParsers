#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "errorDefs.h"
#include "regionParser.h"

//https://minecraft.fandom.com/wiki/Region_file_format

//Program for extracting all nbts from a savefile
int main(int argc, char** argv){
    if(argc < 2){
        argCountError();
    }
    //foreach argument
    for(int i = 2; i < argc; i+=2){
        printf("Starting extraction of %s to %s\n", argv[i - 1], argv[i]);
        char* filename = argv[i - 1];
        FILE* regionFile = fopen(filename, "r");
        if(regionFile == NULL){ 
            nbtFileError(argv[i - 1]);
        }
        chunk* chunks = getChunks(regionFile);
        fclose(regionFile);
        for(int n = 0; n < chunkN; n++){
            if(chunks[n].offset != 0){
                char* filename = malloc(10 + strlen(argv[i]));
                sprintf(filename, "%s/%d.nbt", argv[i], chunks[n].offset);
                FILE* chunkFile = fopen(filename, "w");
                free(filename);
                fwrite(chunks[n].data, chunks[n].byteLength, 1, chunkFile);
                free(chunks[n].data);
                fclose(chunkFile);
            }   
        }
        free(chunks);
        printf("Done\n");
    }
    return 0;
}