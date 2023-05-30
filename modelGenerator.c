#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <math.h>

#include "errorDefs.h"
#include "model.h"
#include "chunkParser.h"

//moved this here from model.c in order to properly separate that file into a proper 3d model rendering lib and a chunk nbt handling lib

//Creates a new cube object based on a block object
struct cube cubeFromBlock(struct block block, const int side, struct material* material);

int main(int argc, char** argv){
    if(argc < 2){
        argCountError();
    }
    char yLim = 0; //if we wan't to remove some verticality
    int upLim = 0; //y+ cutoff
    int downLim = 0; //y- cutoff
    char f = 0; //if we don't want to cull faces
    char b = 0; //if we don't want to cull chunk border faces
    char* materialFilename = NULL;
    int side = 2;
    //argument interface
    for(int i = 2; i < argc; i++){
        if(strcmp(argv[i], "-l") == 0){
            if(argc <= i + 2){
                argError("-l", "2");
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
            printf("modelGenerator <path to nbt file> <arg1> <arg2> ...\nArgs:\n-l <y+> <y-> |limits the result to the given vertical range\n-b|enables chunk border rendering\n-f|disables face culling\n-s <s> |changes the block side in the result side to the given s argument\n-m <filename> | sets the given filename as the .mtl source\n");
            return 0;
        }
        else if(strcmp(argv[i], "-s") == 0){
            if(argc <= i + 1){
                argError("-s", "1");
            }
            side = atoi(argv[i + 1]);
            i+=1;
        }
        else if(strcmp(argv[i], "-m") == 0){
            if(argc <= i + 1){
                argError("-m", "1");
            }
            materialFilename = argv[i + 1];
            i += 1;
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
    unsigned char* data = malloc(sz); //raw NBT file bytes
    fread(data, sz, 1, nbtFile);
    fclose(nbtFile);
    //Array of sections in this chunk
    struct section sections[maxSections] = {};
    int n = getSections(data, sz, sections);
    //It is possible to not have to iterate over each block again, and do everything in a single loop.
    //But that would be less readable and put more of a strain on memory since the entire nbt file would have to be there
    model newModel = initModel(16,16 * n, 16);

    int materialLen = 0; //length of materials
    struct material* materials = NULL; //array of materials
    if(materialFilename != NULL){
        //parse the material file, afterall we have to account for transparent textures
        FILE* mtl = fopen(materialFilename, "r");
        materials = getMaterials(mtl, &materialLen);
        fclose(mtl);
    } 

    //now we have to decrypt the data in sections
    for(int i = 0; i < n; i++){
        //create the block state array
        unsigned int* states = getBlockStates(sections[i], NULL);
        free(sections[i].blockData);
        //if we want to do face culling we first need to actually have all the blocks in one place
        for(int x = 0; x < 16; x++){
            for(int y = 0; y < 16; y++){
                for(int z = 0; z < 16; z++){
                    struct block newBlock = createBlock(x, y, z, states, sections[i]);
                    if((newBlock.y > upLim || newBlock.y < downLim) && yLim){
                        newBlock.type = mcAir;
                    }
                    struct material* m = NULL;
                    if(materialFilename != NULL){
                        char* nameEnd = strchr(newBlock.type, ':');
                        if(nameEnd != NULL){
                            nameEnd++;
                        }
                        else{
                            nameEnd = newBlock.type;
                        }
                        for(int n = 0; n < materialLen; n++){
                            if(strcmp(materials[n].name, nameEnd) == 0){
                                m = &materials[n];
                            }
                        }
                        if(m == NULL && strcmp(newBlock.type, mcAir) != 0){ //material wasn't found oops
                            materialWarning(newBlock.type);
                        }
                    }
                    newModel.cubes[x][y + ((sections[i].y + 4) * 16)][z] = cubeFromBlock(newBlock, side, m);
                }
            }
        }
        free(states);
    }
    if(!f){
        cullFaces(&newModel, !b, mcAir, materials, materialLen);
        printf("Model faces culled\n");
    }
    size_t size = 0;
    char* content = generateModel(&newModel, &size, mcAir, materialFilename);
    freeModel(&newModel);
    freeSections(sections, n);
    for(int i = 0; i < materialLen; i++){
        free(materials[i].name);
    }
    if(materialFilename != NULL){
        free(materials);
    }
    printf("Model string generated\n");
    FILE* outFile = fopen("out.obj", "wb");
    fwrite(content, size, 1, outFile);
    fclose(outFile);
    return 0;
}

struct cube cubeFromBlock(struct block block, const int side, struct material* material){
    struct cube newCube;
    float dist = side/2;
    newCube.side = side;
    newCube.x = (block.x * side) + dist;
    newCube.y = (block.y * side) + dist;
    newCube.z = (block.z * side) + dist;
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

    newCube.m = material;

    newCube.type = block.type;
    return newCube;
}
