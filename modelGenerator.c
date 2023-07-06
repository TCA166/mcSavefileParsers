#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <math.h>

#include "errorDefs.h"
#include "modelGenerator.h"

int main(int argc, char** argv){
    if(argc < 2){
        argCountError();
    }
    bool yLim = 0; //if we wan't to remove some verticality
    int upLim = 0; //y+ cutoff
    int downLim = 0; //y- cutoff
    bool f = 0; //if we don't want to cull faces
    bool b = 0; //if we don't want to cull chunk border faces
    char* objFilename = NULL;
    char* materialFilename = NULL;
    int side = 2;
    char* outFilename = "out.obj";
    //argument interface
    for(int i = 2; i < argc; i++){
        if(strcmp(argv[i], "-l") == 0){
            if(argc <= i + 2){
                argError("-l", "2");
            }
            yLim = true;
            upLim = atoi(argv[i + 1]);
            downLim = atoi(argv[i + 2]);
            printf("Enabled vertical limit from y=%d to y=%d\n", downLim, upLim);
            i += 2;
        }
        else if(strcmp(argv[i], "-f") == 0){
            f = true;
        }
        else if(strcmp(argv[i], "-b") == 0){
            b = true;
        }
        else if(strcmp(argv[i], "-h") == 0){
            printf("modelGenerator <path to nbt file> <arg1> <arg2> ...\nArgs:\n-l <y+> <y-> |limits the result to the given vertical range\n-b|enables chunk border rendering\n-f|disables face culling\n-s <s> |changes the block side in the result side to the given s argument\n-m <filename> | sets the given filename as the .mtl source\n-o <filename> | sets the given filename as special object source\n-out <filename> | sets the given filename as the output filename");
            return EXIT_SUCCESS;
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
            objFilename = argv[i + 1];
            i += 1;
        }
        else if(strcmp(argv[i], "-out") == 0){
            if(argc <= i + 1){
                argError("-out", "1");
            }
            outFilename = argv[i + 1];
            i += 1;
        }
    }
    //Ok so first we get the auxiliary files and then the main file
    hashTable* materials = NULL; //array of materials
    if(materialFilename != NULL){
        //parse the material file, after-all we have to account for transparent textures
        materials = getMaterials(materialFilename);
    } 
    /*Note on the objects
    So generally the workflow looks thusly: file->sections->blocks->cubes->objects->model
    Naturally I could just not use cubes at all and go straight to objects.
    This would indeed be faster and lower the complexity from ~O(6n) to O(5n).
    However that would mean the face culling algorithm would be severely slower.
    Instead of the simple algorithm that assumes everything is a cube and has 6 faces it would need to iterate over each face of each neighboring object to determine if a single face can be culled.
    That's bad. So for now I much rather do one more iteration rather than make culling slow.
    */
    hashTable* objects = NULL; //special objects
    if(objFilename != NULL){
        objects = readWavefront(objFilename, materials, side);
    }
    //Get the nbt data
    FILE* nbtFile = fopen(argv[1], "rb");
    if(nbtFile == NULL){
        fileError(argv[1], "located");
    }
    if(fseek(nbtFile, 0L, SEEK_END) != 0){
        fileError(argv[1], "seek");
    }
    long sz = ftell(nbtFile);
    if(fseek(nbtFile, 0, SEEK_SET) != 0){
        fileError(argv[1], "seek");
    }
    unsigned char* data = malloc(sz); //raw NBT file bytes
    if(fread(data, sz, 1, nbtFile) != 1){
        fileError(argv[1], "read");
    }
    if(fclose(nbtFile) == EOF){
        fileError(argv[1], "closed");
    }
    model newModel = generateFromNbt(data, sz, materials, objects, yLim, upLim, downLim, b, f, side);
    size_t size = 0;
    char* content = generateModel(&newModel, &size, materialFilename);
    freeModel(&newModel);
    printf("Model string generated\n");
    FILE* outFile = fopen(outFilename, "w");
    if(outFile == NULL){
        fileError(outFilename, "opened");
    }
    if(fwrite(content, size, 1, outFile) != 1){
        fileError(outFilename, "written");
    }
    if(fclose(outFile) == EOF){
        fileError(outFilename, "closed");
    }
    free(content);
    return EXIT_SUCCESS;
}

model generateFromNbt(unsigned char* data, long dataSize, hashTable* materials, hashTable* objects, bool yLim, int upLim, int downLim, bool b, bool f, int side){
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
    struct cubeModel cubeModel = createCubeModel(sections, n, materials, yLim, upLim, downLim, side, objects == NULL);
    /*
    Here we transfer the used materials to an array to save memory since we aren't going to be looking up stuff anymore
    The alternative would be to free the entirety of materials now but have copies stored in objects. That probably would be worse
    */
    void** materialsArr = NULL;
    int materialsLen = 0;
    if(materials != NULL){
        /*
        forHashTableItem(materials){
            struct material* mat = (struct material*)item->value;
            free(mat->name);
            free(mat);
        }*/
        materialsArr = hashTableToArray(materials);
        materialsLen = materials->count;
        freeHashTable(materials);
    }
    if(!f){
        long count = cullFaces(&cubeModel, !b, objects);
        printf("%ld model faces culled\n", count);
    }
    model newModel = cubeModelToModel(&cubeModel, objects);
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
    //Free the compressed array of materials
    for(int i = 0; i < materialsLen; i++){
        struct material* m = (struct material*)materialsArr[i];
        if(m != NULL){
            free(m->name);
            free(m);
        }
    }
    free(materialsArr);
    freeSections(sections, n);
    return newModel;
}

struct cubeModel createCubeModel(struct section* sections, int sectionLen, hashTable* materials, bool yLim, int upLim, int downLim, int side, bool matCheck){
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
