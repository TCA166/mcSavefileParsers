#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "portable_endian.h"

#include <math.h>

#include "./cNBT/nbt.h"
#include "./cNBT/list.h"

#include "model.h"

#define maxSections 24

#define createMask(startBit, X) (((long)1 << X) - 1) << startBit

#define nbtTypeError(type, expected) \
        fprintf(stderr, "Expected type:%d, current type:%d.", expected, type);\
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

#define nbtTagError(tag) \
        fprintf(stderr, "Tag %s was not found.", tag);\
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

#define fileError(filename) \
        fprintf(stderr, "File %s couldn't be located.", filename); \
        perror("File error.");\
        exit(EXIT_FAILURE);

#define statesError(state, paletteLen, block) \
        fprintf(stderr, "%d > %d:states error\n", state, paletteLen); \
        block.type = mcAir;

//16x16x16 big section of a chunk
struct section{
    short y;

    //we ignore the biomes because we don't need that data

    unsigned long* blockData;
    int blockDataLen;
    char** blockPalette;
    int paletteLen;
};

int main(int argc, char** argv){
    char yLim = 0; //if we wan't to remove some verticality
    int upLim = 0; //y+ cutoff
    int downLim = 0; //y- cutoff
    char f = 0; //if we don't want to cull faces
    char b = 0; //if we don't want to cull chunk border faces
    int side = 2;
    //argument interface
    for(int i = 2; i < argc; i++){ //so far not much args... maybe in future more will be added
        if(strcmp(argv[i], "-l") == 0){
            if(argc <= i + 2){
                fprintf(stderr, "Incorrect number of arguments. -l requires two arguments to follow.");
                break;
            }
            yLim = 1;
            upLim = atoi(argv[i + 1]);
            downLim = atoi(argv[i + 2]);
            i += 2;
        }
        else if(strcmp(argv[i], "-f") == 0){
            f = 1;
        }
        else if(strcmp(argv[i], "-b") == 0){
            b = 1;
        }
        else if(strcmp(argv[i], "-h") == 0){
            printf("modelGenerator <path to nbt file> <arg1> <arg2> ...\nArgs:\n-l <y+> <y-> |limits the result to the given vertical range\n-b|enables chunk border rendering\n-f|disables face culling\n-s <s> |changes the block side in the result side to the given s argument\n");
            return 0;
        }
        else if(strcmp(argv[i], "-s") == 0){
            if(argc <= i + 1){
                fprintf(stderr, "Incorrect number of arguments. -s requires an argument to follow.");
                break;
            }
            side = atoi(argv[i + 1]);
        }
    }
    //Get the nbt data
    FILE* nbtFile = fopen(argv[1], "r");
    if(nbtFile == NULL){
        fileError(argv[1]);
    }
    fseek(nbtFile, 0L, SEEK_END);
    long sz = ftell(nbtFile);
    fseek(nbtFile, 0, SEEK_SET);
    unsigned char* data = malloc(sz);
    fread(data, sz, 1, nbtFile);
    fclose(nbtFile);
    //parse it
    nbt_node* node = nbt_parse(data, sz);
    if(errno != 0){
        fprintf(stderr, "%d\n", errno);
        perror("Error while reading the nbt file");
        exit(EXIT_FAILURE);
    }
    if(node->type != TAG_COMPOUND){
        nbtTypeError(node->type, 10);
    }
    //Array of sections in this chunk
    struct section sections[maxSections] = {};
    //Debug message   
    const struct list_head* head = &node->payload.tag_compound->entry;
    printf("NBT file has %zu tags\n", list_length(head));
    //get the sections tag
    nbt_node* sectionsNode = nbt_find_by_name(node, "sections");
    if(sectionsNode == NULL){
        nbtTagError("sections");
    }
    struct nbt_list* sectionsList = sectionsNode->payload.tag_list;
    struct list_head* pos = &sectionsList->entry;
    int n = 0;
    //foreach section
    list_for_each(pos, &sectionsList->entry){ 
        //get the element
        struct nbt_list* el = list_entry(pos, struct nbt_list, entry);
        nbt_node* compound = el->data;
        struct section newSection; //create new object that will store this data
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
            newSection.blockData = malloc(blockData->payload.tag_long_array.length * sizeof(long));
            memcpy(newSection.blockData, blockData->payload.tag_long_array.data, blockData->payload.tag_long_array.length * sizeof(long));
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
        const struct list_head* palleteHead = &palette->payload.tag_list->entry;
        char** blockPalette = malloc(1 * sizeof(char*));
        int i = 0;
        struct list_head* paletteCur = &palette->payload.tag_list->entry;
        //foreach element in palette
        list_for_each(paletteCur, &palette->payload.tag_list->entry){
            //get the list entry
            struct nbt_list* pal = list_entry(paletteCur, struct nbt_list, entry);
            nbt_node* string = nbt_find_by_name(pal->data, "Name");
            if(string == NULL){
                nbtTagError("Name");
            }
            blockPalette[i] = malloc(strlen(string->payload.tag_string) + 1);
            strcpy(blockPalette[i], string->payload.tag_string);
            i++;
            blockPalette = realloc(blockPalette, (i + 1) * sizeof(char*));
        }
        newSection.blockPalette = blockPalette;
        newSection.paletteLen = i;
        sections[n] = newSection;
        n++;
    }
    //ok we got out all the section data time to free it all
    nbt_free(node);
    //It is possible to not have to iterate over each block again, and do everything in a single loop.
    //But that would be less readable and put more of a strain on memory since the entire nbt file would have to be there
    model newModel = initModel(16,16 * n, 16);
    //now we have to decrypt the data in sections
    for(int i = 0; i < n; i++){
        short l = (short)ceilf(log2f((float)sections[i].paletteLen));//length of indices in the long
        //first we need to decode the franken compression scheme
        unsigned int* states = NULL;
        if(sections[i].paletteLen != 1){
            int m = 0;
            short count = 64/l * sections[i].blockDataLen; //amount of indices in each long
            /*
            if(count < 4){
                count = 4;
            }*/
            states = malloc(count * sizeof(unsigned int));
            //foreach long
            for(int a=0; a < sections[i].blockDataLen; a++){
                unsigned long comp = sections[i].blockData[a];
                //uint64_t htobe64(uint64_t host_64bits);
                //unsigned long bigComp = htobe64(comp);
                //foreach set of l bits
                for(short b = 0; b + l < 64; b+=l){
                    unsigned long mask = createMask(b, l);
                    states[m] = (unsigned int)((mask & comp) >> b);
                    if(states[m] > sections[i].paletteLen){
                        states[m] = sections[i].paletteLen - 1;
                    }
                    m++;
                }
            }
        }
        free(sections[i].blockData);
        //if we want to do face culling we first need to actually have all the blocks in one place
        for(int x = 0; x < 16; x++){
            for(int y = 0; y < 16; y++){
                for(int z = 0; z < 16; z++){
                    int blockPos = (y * 16 + z) * 16 + x; //4096 + 256 + 16 = 4368
                    struct block newBlock;
                    newBlock.x = x * side;
                    int arrY = y + ((sections[i].y + 4) * 16);
                    newBlock.y = arrY * side;
                    newBlock.z = z * side;
                    if(!(newBlock.y > downLim && newBlock.y < upLim) && yLim){
                        newBlock.type = mcAir;
                    }
                    if(sections[i].paletteLen == 1){
                        newBlock.type = sections[i].blockPalette[0];
                    }
                    else{
                        int state = states[blockPos];
                        //paletteLen and I are fine it must be something with the data extraction process
                        if(states[blockPos] >= sections[i].paletteLen){
                            statesError(states[blockPos], sections[i].paletteLen, newBlock);
                        }
                        else{
                            newBlock.type = sections[i].blockPalette[states[blockPos]];
                        }
                        
                    }
                    newModel.cubes[x][arrY][z] = cubeFromBlock(newBlock, side);
            }
            }
        }
        free(states);
    }
    if(!f){
        cullFaces(&newModel, !b);
        printf("Model faces culled\n");
    }
    size_t size = 0;
    char* content = generateModel(&newModel, &size);
    freeModel(&newModel);
    printf("Model string generated\n");
    FILE* outFile = fopen("out.obj", "wb");
    fwrite(content, size, 1, outFile);
    fclose(outFile);
    return 0;
}
