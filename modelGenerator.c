#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include <math.h>

#include "errorDefs.h"
#include "generator.h"

int main(int argc, char** argv){
    if(argc < 2){
        argCountError();
    }
    bool yLim = false; //if we wan't to remove some verticality
    int upLim = 0; //y+ cutoff
    int downLim = 0; //y- cutoff
    bool f = false; //if we don't want to cull faces
    bool b = false; //if we don't want to cull chunk border faces
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
    model newModel = generateFromNbt(data, sz, materials, objects, yLim, upLim, downLim, b, f, side, 0, 0);
    size_t size = 0;
    //chunk can hold a maximum of 98304 objects
    //Size of newModel is int*3 + 16*384*16*2*ptr bytes max in a chunk
    printf("Generating %dx%dx%d model\n", newModel.x, newModel.y, newModel.z);
    char* content = generateModel(&newModel, &size, materialFilename, NULL);
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
