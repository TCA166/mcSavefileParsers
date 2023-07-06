#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "errorDefs.h"
#include "generator.h"
#include "regionParser.h"
//I'm gonna need to do some 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv){
    //very similar to modelGenerator
    if(argc < 5){
        argCountError();
    }
    bool yLim = false; //if we want to 
    int upLim = 0;
    int downLim = 0;
    char* objFilename = NULL;
    char* materialFilename = NULL;
    int side = 2;
    char* outFilename = "out.obj";
    char* regionDirPath = argv[1];
    int xCenter = atoi(argv[2]); //the x chunk coordinate that will be the start of our radius
    int zCenter = atoi(argv[3]);
    int radius = atoi(argv[4]); 
    if(radius < 1){
        argValError("radius");
    }
    for(int i = 5; i < argc; i++){
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
        else if(strcmp(argv[i], "-h") == 0){
            printf("radiusGenerator <path to region directory> <x> <z> <radius> ...\nArgs:\n-l <y+> <y-> |limits the result to the given vertical range\n-s <s> |changes the block side in the result side to the given s argument\n-m <filename> | sets the given filename as the .mtl source\n-o <filename> | sets the given filename as special object source\n-out <filename> | sets the given filename as the output filename");
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
    //Ok so first we get the auxiliary file
    hashTable* materials = NULL; 
    if(materialFilename != NULL){
        materials = getMaterials(materialFilename);
    } 
    hashTable* objects = NULL; //special objects
    if(objFilename != NULL){
        objects = readWavefront(objFilename, materials, side);
    }
    //and now the big thing
    int numChildren = ((xCenter + radius) - (xCenter - radius)) * ((zCenter + radius) - (zCenter - radius));
    int** fd = calloc(numChildren, sizeof(int[2]));
    int counter = 0;
    pid_t child_pid, wpid;
    int status = 0; //status container
    //foreach chunk in radius
    for(int x = xCenter - radius; x <= xCenter + radius; x++){
        for(int z = zCenter - radius; z <= zCenter + radius; z++){
            if((child_pid = fork()) == 0){
                //child process code
                chunk ourChunk = extractChunk(regionDirPath, x, z);
                model partModel = generateFromNbt(ourChunk.data, ourChunk.byteLength, materials, objects, yLim, upLim, downLim, true, false, side);
                //well now we have to pipe the model to the parent and that's gonna be a mess
                for(int mx = 0; mx < partModel.x; mx++){
                    for(int my = 0; my < partModel.y; my++){
                        for(int mz = 0; mz < partModel.z; mz++){
                            
                        }
                    }
                }
                write(fd[counter][1], &partModel, sizeof(model));
                exit(EXIT_SUCCESS);
            }
            counter++;
        }
    }
    //parent process code
    while((wpid = wait(&status)) > 0){

    }
    exit(EXIT_SUCCESS);
}