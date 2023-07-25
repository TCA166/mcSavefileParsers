#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "./cNBT/nbt.h"
#include "./cNBT/list.h"

#include "errorDefs.h"
#include "chunkParser.h"

#include <inttypes.h>

#define createMask(startBit, X) ((((uint64_t)1) << X) - 1) << startBit

#define statesFormula(x, y, z) (y*16*16) + (z*16) + x

char* getProperty(const char* property, nbt_node* tree){
    char* result = NULL;
    nbt_node* colorNode = nbt_find_by_name(tree, property);
    if(colorNode != NULL){
        result = malloc(strlen(colorNode->payload.tag_string) + 1);
        strcpy(result, colorNode->payload.tag_string);
    }
    return result;
}

char* appendProperty(char* string, char* property, const char* propertyName){
    string = realloc(string, strlen(string) + 3 + strlen(property) + strlen(propertyName));
    strcat(string, propertyName);
    strcat(string, "=");
    strcat(string, property);
    strcat(string, ",");
    free(property);
    return string;
}

unsigned int getSections(unsigned char* nbtFileData, long sz, section* sections){
    nbt_node* node = nbt_parse(nbtFileData, sz);
    if(errno != 0){
        cNBTError("the nbt file")
    }
    if(node->type != TAG_COMPOUND){
        nbtTypeError(node->type, 10);
    }
    //Debug message   
    //const struct list_head* head = &node->payload.tag_compound->entry;
    //printf("NBT file has %zu tags\n", list_length(head));
    //get the sections tag
    nbt_node* sectionsNode = nbt_find_by_name(node, "sections");
    if(sectionsNode == NULL){
        nbtTagError("sections");
    }
    struct nbt_list* sectionsList = sectionsNode->payload.tag_list;
    struct list_head* pos;
    int n = 0;
    //foreach section
    list_for_each(pos, &sectionsList->entry){ 
        //get the element
        struct nbt_list* el = list_entry(pos, struct nbt_list, entry);
        nbt_node* compound = el->data;
        section newSection; //create new object that will store this data
        //get the Y
        nbt_node* yNode = nbt_find_by_name(compound, "Y");
        if(yNode == NULL){
            nbtTagError("Y");
        }
        newSection.y = yNode->payload.tag_byte;
        //get the block data
        nbt_node* blockNode = nbt_find_by_name(compound, "block_states");
        if(blockNode == NULL){
            nbtTagError("block_states");
        }
        //get the individual block data
        nbt_node* blockData = nbt_find_by_name(blockNode, "data");
        if(blockData != NULL){
            newSection.blockData = (uint64_t*)malloc(blockData->payload.tag_long_array.length * sizeof(uint64_t));
            memcpy(newSection.blockData, blockData->payload.tag_long_array.data, blockData->payload.tag_long_array.length * sizeof(int64_t));
            newSection.blockDataLen = blockData->payload.tag_long_array.length;
        }
        else{ //it can be null in which case the entire sector is full of palette[0]
            newSection.blockData = NULL;
            newSection.blockData = 0;
        }
        //get the palette
        nbt_node* palette = nbt_find_by_name(blockNode, "palette");
        if(palette == NULL){
            nbtTagError("palette");
        }
        //const struct list_head* paletteHead = &palette->payload.tag_list->entry;
        char** blockPalette = malloc(sizeof(char*));
        unsigned int i = 0;
        struct list_head* paletteCur;
        //foreach element in palette
        list_for_each(paletteCur, &palette->payload.tag_list->entry){
            //get the list entry
            struct nbt_list* pal = list_entry(paletteCur, struct nbt_list, entry);
            nbt_node* string = nbt_find_by_name(pal->data, "Name");
            if(string == NULL){
                nbtTagError("Name");
            }
            //Instead of appending all properties in block using a for loop we extract a few specific ones
            char* direction = NULL;
            char* half = NULL;
            char* shape = NULL;
            char* color = NULL;
            char* part = NULL;
            char* open = NULL;
            char* snowy = NULL;
            char* north = NULL;
            char* west = NULL;
            char* east = NULL;
            char* south = NULL;
            nbt_node* properties = nbt_find_by_name(pal->data, "Properties");
            if(properties != NULL){
                color = getProperty("color", properties);
                east = getProperty("east", properties);
                direction = getProperty("facing", properties);
                half = getProperty("half", properties);
                north = getProperty("north", properties);
                open = getProperty("open", properties);
                part = getProperty("part", properties);
                shape = getProperty("shape", properties);
                snowy = getProperty("snowy", properties);
                south = getProperty("south", properties);
                west = getProperty("west", properties);
            }
            blockPalette[i] = malloc(strlen(string->payload.tag_string) + 1);
            strcpy(blockPalette[i], string->payload.tag_string);
            char* propertyString = malloc(2);
            propertyString[0] = ';';
            propertyString[1] = '\0';
            if(color != NULL){
                propertyString = appendProperty(propertyString, color, "color");
            }
            if(east != NULL){
                propertyString = appendProperty(propertyString, east, "east");
            }
            if(direction != NULL){
                propertyString = appendProperty(propertyString, direction, "facing");
            }
            if(half != NULL){
                propertyString = appendProperty(propertyString, half, "half");
            }
            if(north != NULL){
                propertyString = appendProperty(propertyString, north, "north");
            }
            if(open != NULL){
                propertyString = appendProperty(propertyString, part, "open");
            }
            if(part != NULL){
                propertyString = appendProperty(propertyString, part, "part");
            }
            if(shape != NULL){
                propertyString = appendProperty(propertyString, shape, "shape");
            }
            if(snowy != NULL){
                propertyString = appendProperty(propertyString, snowy, "snowy");
            }
            if(south != NULL){
                propertyString = appendProperty(propertyString, south, "south");
            }
            if(west != NULL){
                propertyString = appendProperty(propertyString, west, "west");
            }
            propertyString[strlen(propertyString) - 1] = '\0';
            blockPalette[i] = realloc(blockPalette[i], strlen(blockPalette[i]) + 1 + strlen(propertyString));
            strcat(blockPalette[i], propertyString);
            free(propertyString);
            i++;
            blockPalette = realloc(blockPalette, (i + 1) * sizeof(char*));
        }
        newSection.blockPalette = blockPalette;
        if(i == UINT32_MAX){
            overflowError("UINT32");
        }
        newSection.paletteLen = i;
        sections[n] = newSection;
        n++;
    }
    //ok we got out all the section data time to free it all
    nbt_free(node);
    return n;
}

unsigned int* getBlockStates(section s, int* outLen){
    //first we need to decode the franken compression scheme
    unsigned int* states = NULL;
    if(s.paletteLen > 1){
        unsigned short l = (unsigned short)ceilf(log2f((float)s.paletteLen));//length of indices in the long
        //I did the math and we could have a problem if l>32. However for that to happen paletteLen would have to be 3999999999, which is larger than INT_MAX, and impossible because Minecraft has 820 blocks total
        int m = 0;
        if(l < 4){
            l = 4;
        }
        unsigned short numPerLong = (unsigned short)(64/l);
        //fprintf(stderr, "%d %d\n", l, numPerLong);
        states = malloc(numPerLong * s.blockDataLen * sizeof(unsigned int));
        //foreach long
        for(int a=0; a < s.blockDataLen; a++){
            uint64_t comp = s.blockData[a];
            for(unsigned short b = 0; b < numPerLong; b++){
                if(m >= 4096){
                    break;
                }
                unsigned short bits = b * l;
                uint64_t mask = createMask(bits, l);
                states[m] = (unsigned int)((mask & comp) >> bits);
                //fprintf(stderr, "%ld&%ld=%ld, >>%d=%d\n", mask, comp, (mask & comp), bits, states[m]);
                m++;
            }
        }
        if(outLen != NULL){
            *outLen = m;
        }
    }
    return states;
}

block createBlock(int x, int y, int z, unsigned int* blockStates, section parentSection){
    block newBlock;
    int blockPos = statesFormula(x, y, z);
    newBlock.x = x;
    newBlock.y = y + ((parentSection.y + 4) * 16);
    newBlock.z = z;
    //if we can look up the block state in the array
    if(blockStates == NULL){
        newBlock.type = parentSection.blockPalette[0];
    }
    else{
        int state = blockStates[blockPos];
        //paletteLen and I are fine it must be something with the data extraction process
        if(state >= parentSection.paletteLen){
            statesError(state, parentSection.paletteLen, newBlock);
            newBlock.type = mcAir;
        }
        else{
            newBlock.type = parentSection.blockPalette[state];
        }
        
    }
    return newBlock;
}

bool contains(char** arr, char* str, int arrLen){
    for(int i = 0; i < arrLen; i++){
        if(strcmp(arr[i], str) == 0){
            return true;
        }
    }
    return false;
}

//this code feels wrong to write in C ngl...
char** createGlobalPalette(section* sections, int len, int* outLen, bool freeSectionPalettes){
    char** globalPalette = NULL;
    int j = 0;
    for(int i = 0; i < len; i++){
        for(int n = 0; n < sections[i].paletteLen; n++){
            if(!contains(globalPalette, sections[i].blockPalette[n], j)){
                globalPalette = realloc(globalPalette, (j + 1) * sizeof(char*));
                globalPalette[j] = sections[i].blockPalette[n];
                j++;
            }
        }
        if(freeSectionPalettes){
            free(sections[i].blockPalette);
        }
    }
    *outLen = j;
    return globalPalette;
}

void freeSections(section* sections, int sectionLen){
    for(int i = 0; i < sectionLen; i++){
        for(int n = 0; n < sections[i].paletteLen; n++){
            free(sections[i].blockPalette[n]);
        }
        free(sections[i].blockPalette);
    }
}
