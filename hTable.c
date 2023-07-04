#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "hTable.h"

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define getIndex(key, table) (size_t)(hash(key) & (uint64_t)(table->size - 1))

//taken from https://benhoyt.com/writings/hash-table-in-c/
//i am not smart enough to come up with my own hash function

//Generates a hash from a string
uint64_t hash(const char* key)
{
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

struct hTableItem* initHashItem(const char* key, const void* value){
    struct hTableItem* item = malloc(sizeof(struct hTableItem));
    item->key = malloc(strlen(key) + 1);
    item->value = (void*)value;
    strcpy(item->key, key);
    item->next = NULL;
    return item;
}

hashTable* initHashTable(size_t size){
    hashTable* table = malloc(sizeof(hashTable));
    table->count = 0;
    table->size = size;
    table->items = calloc(size, sizeof(struct hTableItem*));
    return table;
}

void freeHashItem(struct hTableItem* item){
    free(item->key);
    free(item);
}

void freeHashTable(hashTable* table){
    for(int i = 0; i < table->size; i++){
        struct hTableItem* item = table->items[i];
        if(item != NULL){
            freeHashItem(item);
        }
    }
    free(table->items);
    free(table);
}

int insertHashItem(hashTable* table, const char* key, const void* value){
    int index = getIndex(key, table);
    /*
    if(index > table->size){
        int oldSize = table->size;
        table->size = index;
        //re-calloc
        table->items = realloc(table->items, index * sizeof(struct hTableItem*));
        for(int i = oldSize; i < table->size; i++){
            table->items[i] = NULL;
        }
    }*/
    struct hTableItem* currentItem = table->items[index]; //currently stored item
    while(currentItem != NULL){
        if(strcmp(key, currentItem->key) == 0){
            currentItem->value = (void*)value;
            return table->count;
        }
        else{
            currentItem = currentItem->next;
        }
    }
    struct hTableItem* newItem = initHashItem(key, value);
    table->items[index] = newItem;
    table->count++;
    return table->count;
}

void* getVal(hashTable* table, const char* key){
    if(table == NULL){
        return NULL;
    }
    int index = getIndex(key, table);
    struct hTableItem* item = table->items[index];
    if(item == NULL){
        return NULL;
    }
    while(strcmp(key, item->key) != 0){
        item = item->next;
        if(item == NULL){
            return table->items[index];
        }
    }
    return item->value;
}
