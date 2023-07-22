#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <math.h>

#include "errorDefs.h"
#include "generator.h"

model generateFromNbt(unsigned char* data, long dataSize, hashTable* materials, hashTable* objects, bool yLim, int upLim, int downLim, bool b, bool f, int side, int chunkX, int chunkZ){
    //Array of sections in this chunk
    struct section sections[maxSections] = {0};
    int n = getSections(data, dataSize, sections);
    free(data);
    /*It is possible to not have to iterate over each block again, and do everything in a single loop.
    But that would be less readable and put more of a strain on memory since the entire nbt file would have to be there
    Also it's C already. Just by the virtue of doing it in C I'm pretty fast.
    Also also the resulting API and code would be less open and more goal centric, which is something I don't really want. Hey if I create code for handling obj file in C why not make it reusable?
    */
    //now we have to decrypt the data in sections
    struct cubeModel cubeModel = createCubeModel(sections, n, materials, yLim, upLim, downLim, side, objects == NULL, chunkX * 16, chunkZ * 16);
    /*
    Here we transfer the used materials to an array to save memory since we aren't going to be looking up stuff anymore
    The alternative would be to free the entirety of materials now but have copies stored in objects. That probably would be worse
    */
    struct material** materialsArr = NULL;
    int materialsLen = 0;
    if(materials != NULL){
        /*
        forHashTableItem(materials){
            struct material* mat = (struct material*)item->value;
            free(mat->name);
            free(mat);
        }*/
        materialsArr = (struct material**)hashTableToArray(materials);
        materialsLen = materials->count;
        freeHashTable(materials);
    }
    if(!f){
        long count = cullFaces(&cubeModel, !b, objects);
        printf("%ld model faces culled\n", count);
    }
    model newModel = cubeModelToModel(&cubeModel, objects);
    newModel.materialArr = materialsArr;
    newModel.materialCount = materialsLen;
    //We don't need objects, they have already been copied in cubeModelToModel so let's free that hash table
    if(objects != NULL){
        forHashTableItem(objects){
            struct object* obj = (struct object*)item->value;
            free(obj->type);
            free(obj->vertices);
            for(int i = 0; i < obj->faceCount; i++){
                free(obj->faces[i].vertices);
            }
            free(obj->faces);
            free(obj);
        }
        freeHashTable(objects);
    }
    freeCubeModel(&cubeModel);
    freeSections(sections, n);
    return newModel;
}

struct cubeModel createCubeModel(struct section* sections, int sectionLen, hashTable* materials, bool yLim, int upLim, int downLim, int side, bool matCheck, int xOff, int zOff){
    struct cubeModel cubeModel = initCubeModel(16,16 * sectionLen, 16);
    for(int i = 0; i < sectionLen; i++){
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
                    newBlock.x += xOff;
                    newBlock.z += zOff;
                    struct material* m = NULL;
                    if(materials != NULL){
                        char* nameEnd = strchr(newBlock.type, ':');
                        if(nameEnd != NULL){
                            nameEnd++;
                        }
                        else{
                            nameEnd = newBlock.type;
                        }
                        m = (struct material*)getVal(materials, nameEnd);
                        if(m == NULL && strcmp(newBlock.type, mcAir) != 0 && matCheck){ //material wasn't found oops
                            materialWarning(nameEnd);
                        }
                    }
                    if(strcmp(newBlock.type, mcAir) != 0){
                        cubeModel.cubes[x][y + ((sections[i].y + 4) * 16)][z] = malloc(sizeof(struct cube));
                        *(cubeModel.cubes[x][y + ((sections[i].y + 4) * 16)][z]) = cubeFromBlock(newBlock, side, m);
                    }
                    else{
                        cubeModel.cubes[x][y + ((sections[i].y + 4) * 16)][z] = NULL;
                    }
                    
                }
            }
        }
        free(states);
    }
    return cubeModel;
}

struct cube cubeFromBlock(struct block block, const int side, struct material* material){
    struct cube newCube;
    newCube.side = side;
    newCube.x = block.x;
    newCube.y = block.y;
    newCube.z = block.z;
    float dist = side/2;
    //binary 8 to 0
    newCube.vertices[0] = newVertex(dist, dist,  dist);
    newCube.vertices[1] = newVertex(dist, dist, 0 - dist);
    newCube.vertices[2] = newVertex(dist, 0 - dist, dist);
    newCube.vertices[3] = newVertex(dist, 0 - dist, 0 - dist);
    newCube.vertices[4] = newVertex(0 - dist, dist, dist);
    newCube.vertices[5] = newVertex(0 - dist, dist, 0 - dist);
    newCube.vertices[6] = newVertex(0 - dist, 0 - dist, dist);
    newCube.vertices[7] = newVertex(0 - dist, 0 - dist, 0 - dist);
    
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