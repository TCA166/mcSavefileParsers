/*!
 @file blockPrint.c
 @brief Prints the number of each block in a chunk
 @details This program prints the number of each block in a chunk
*/

#include <stdlib.h>
#include <stdio.h>
#include "./lib/errorDefs.h"
#include "./lib/chunkParser.h"
#include "./lib/hTable.h"

/*!
 @brief Loads an nbt file and prints the number of each block in a chunk
 @details the association of block type to block count is stored in a hash table
 @see chunkParser.h
 @see hTable.h 
*/
int main(int argc, char** argv){
    if(argc < 2){
        argCountError("blockPrint <path to nbt file>");
    }
    unsigned char* data;
    long sz;
    {
        FILE* file = fopen(argv[1], "rb");
        if(file == NULL){
            fileError(argv[1], "opened");
        }
        if(fseek(file, 0, SEEK_END) != 0){
            fileError(argv[1], "seeked");
        }
        sz = ftell(file);
        if(fseek(file, 0, SEEK_SET) != 0){
            fileError(argv[1], "seeked");
        }
        data = malloc(sz);
        if(fread(data, sz, 1, file) != 1){
            fileError(argv[1], "read");
        }
        fclose(file);
    }
    section sections[maxSections];
    int sectionN = getSections(data, sz, sections);
    free(data);
    hashTable* blockCount = initHashTable(49152); //max blocks in chunk divided by 2
    for(int i = 0; i < sectionN; i++){
        int outLen;
        unsigned int* blockStates = getBlockStates(sections[i], &outLen);
        if(blockStates == NULL){
            char* key = sections[i].blockPalette[0];
            if(getVal(blockCount, key) == NULL){
                int* val = malloc(sizeof(int));
                insertHashItem(blockCount, key, val);
            }
            else{
                int* val = getVal(blockCount, key);
                *val += 1;
            }
        }
        else{
            for(int j = 0; j < outLen; j++){
                char* key = sections[i].blockPalette[blockStates[j]];
                if(getVal(blockCount, key) == NULL){
                    int* val = malloc(sizeof(int));
                    insertHashItem(blockCount, key, val);
                }
                else{
                    int* val = getVal(blockCount, key);
                    *val += 1;
                }
            }
        }
        free(blockStates);
    }
    freeSections(sections, sectionN);
    forHashTableItem(blockCount){
        printf("%s: %d\n", item->key, *(int*)item->value);
    }
    freeHashTable(blockCount, free);
    return EXIT_SUCCESS;
}
