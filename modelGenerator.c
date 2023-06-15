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

//Isolated from the main code to maybe in the future enable for a multiprocessed version that can do multiple chunks
struct cubeModel createCubeModel(struct section* sections, int sectionLen, struct material* materials, int materialLen, int upLim, int downLim, char yLim, int side);

int main(int argc, char** argv){
    if(argc < 2){
        argCountError();
    }
    char yLim = 0; //if we wan't to remove some verticality
    int upLim = 0; //y+ cutoff
    int downLim = 0; //y- cutoff
    char f = 0; //if we don't want to cull faces
    char b = 0; //if we don't want to cull chunk border faces
    struct object* objects = NULL;
    int objLen = 0;
    char* objFilename = NULL;
    char** specialObjects = NULL;
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
            printf("Enabled vertical limit from y=%d to y=%d\n", downLim, upLim);
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
        else if(strcmp(argv[i], "-o") == 0){
            if(argc <= i + 1){
                argError("-o", "1");
            }
            materialFilename = argv[i + 1];
        }
    }
    //Get the nbt data
    FILE* nbtFile = fopen(argv[1], "rb");
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
    free(data);
    /*It is possible to not have to iterate over each block again, and do everything in a single loop.
    But that would be less readable and put more of a strain on memory since the entire nbt file would have to be there
    Also it's C already. Just by the virtue of doing it in C I'm pretty fast.
    Also also the resulting API and code would be less open and more goal centric, which is something I don't really want. Hey if I create code for handling obj file in C whhy not make it reusable?
    */

    int materialLen = 0; //length of materials
    struct material* materials = NULL; //array of materials
    if(materialFilename != NULL){
        //parse the material file, afterall we have to account for transparent textures
        FILE* mtl = fopen(materialFilename, "r");
        materials = getMaterials(mtl, &materialLen);
        fclose(mtl);
    } 
    /*Note on the objects
    So generally the workflow looks thusly: file->sections->blocks->cubes->objects->model
    Naturally I could just not use cubes at all and go straight to objects.
    This would indeed be faster and lower the complexity from ~O(6n) to O(5n).
    However that would mean the faceculling algorithm would be severely slower.
    Instead of the simple algorithm that assumes everything is a cube and has 6 faces it would need to iterate over each face of each neighboring object to determine if a single face can be culled.
    That's bad. So for now I much rather do one more iteration rather than make culling slow.
    */
    if(objFilename != NULL){
        objects = readWavefront(objFilename, &objLen, materials, materialLen, side);
        specialObjects = malloc(objLen * sizeof(char*));
        for(int i = 0; i < objLen; i++){
            specialObjects[i] = objects[i].type;
        }
    }
    //now we have to decrypt the data in sections
    struct cubeModel cubeModel = createCubeModel(sections, n, materials, materialLen, upLim, downLim, yLim, side);
    if(!f){
        cullFaces(&cubeModel, !b, specialObjects, objLen);
        printf("Model faces culled\n");
    }
    free(specialObjects);
    model newModel = cubeModelToModel(&cubeModel, objects, objLen);
    freeCubeModel(&cubeModel);
    size_t size = 0;
    char* content = generateModel(&newModel, &size, materialFilename);
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
    free(content);
    return 0;
}

struct cubeModel createCubeModel(struct section* sections, int sectionLen, struct material* materials, int materialLen, int upLim, int downLim, char yLim, int side){
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
                    struct material* m = NULL;
                    if(materials != NULL){
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
    float dist = side/2;
    newCube.side = side;
    newCube.x = block.x;
    newCube.y = block.y;
    newCube.z = block.z;
    //fprintf(stderr, "%f %f %f", newCube.x, newCube.y, newCube.z);
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
