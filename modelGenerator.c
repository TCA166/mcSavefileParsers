#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "portable_endian.h"

#include <math.h>

#include "errorDefs.h"
#include "model.h"
#include "chunkParser.h"

//moved this here from model.c in order to properly seperate that file into a proper 3d model rendering lib and a chunk nbt handling lib

//Creates a new cube object based on a block object
struct cube cubeFromBlock(struct block block, const int side){
    struct cube newCube;
    float dist = side/2;
    newCube.side = side;
    newCube.x = block.x + dist;
    newCube.y = block.y + dist;
    newCube.z = block.z + dist;
    //binary 8 to 0
    newCube.vertices[0] = newVertex(newCube.x + dist, newCube.y + dist, newCube.z + dist);
    newCube.vertices[1] = newVertex(newCube.x + dist, newCube.y + dist, newCube.z - dist);
    newCube.vertices[2] = newVertex(newCube.x + dist, newCube.y - dist, newCube.z + dist);
    newCube.vertices[3] = newVertex(newCube.x + dist, newCube.y - dist, newCube.z - dist);
    newCube.vertices[4] = newVertex(newCube.x - dist, newCube.y + dist, newCube.z + dist);
    newCube.vertices[5] = newVertex(newCube.x - dist, newCube.y + dist, newCube.z - dist);
    newCube.vertices[6] = newVertex(newCube.x - dist, newCube.y - dist, newCube.z + dist);
    newCube.vertices[7] = newVertex(newCube.x - dist, newCube.y - dist, newCube.z - dist);
    
    newCube.faces[0] = newCubeFace(1, 0, 2, 3); //right +x
    newCube.faces[1] = newCubeFace(1, 0, 4, 5); //up +y
    newCube.faces[2] = newCubeFace(2, 0, 4, 6); //forward +z
    newCube.faces[3] = newCubeFace(7, 3, 1, 5); //back -z
    newCube.faces[4] = newCubeFace(7, 5, 4, 6); //left -x
    newCube.faces[5] = newCubeFace(7, 6, 2, 3); //down -y

    newCube.type = block.type;
    return newCube;
}

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
    //Array of sections in this chunk
    struct section sections[maxSections] = {};
    int n = getSections(data, sz, sections);
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
