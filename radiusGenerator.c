#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "errorDefs.h"
#include "generator.h"
#include "regionParser.h"

//I'm gonna need to do some macro chicanery to get this working on Windows
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <fcntl.h> 

#define WRITE_END 1
#define READ_END 0

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
    pid_t parentId = getpid();
    //matches the counter after the double for loop has run it's course
    int numChildren = ((xCenter + radius) - (xCenter - radius) + 1) * ((zCenter + radius) - (zCenter - radius) + 1); 
    pid_t* childrenPids = calloc(numChildren, sizeof(pid_t)); //array of children pid, used to connect the counter and the pid later
    int** fd = calloc(numChildren, sizeof(int*)); //array of pipes
    //we create all the pipes
    for(int i = 0; i < numChildren; i++){
        fd[i] = calloc(2, sizeof(int));
        if(pipe(fd[i]) < 0){
            pipeError("", "creation");
        }
    }
    int counter = 0;
    pid_t child_pid, wpid;
    int status = 0; //status container
    //foreach chunk in radius
    for(int x = xCenter - radius; x <= xCenter + radius; x++){
        for(int z = zCenter - radius; z <= zCenter + radius; z++){
            if((child_pid = fork()) > 0){//parent process
                childrenPids[counter] = child_pid;
                close(fd[counter][WRITE_END]);
                counter++;
            }
            else if(child_pid == 0){ //child process
                close(fd[counter][READ_END]);
                chunk ourChunk = extractChunk(regionDirPath, x, z);
                model partModel = generateFromNbt(ourChunk.data, ourChunk.byteLength, materials, objects, yLim, upLim, downLim, true, false, side);
                //ok so now the idea is to use mmap to create a shared buffer, and then pipe the pointer to that buffer
                size_t size = getTotalModelSize(&partModel);
                //we need to create a shared memory segment. We will use that segment to transfer the entire model data.
                int shmid = shmget(parentId + counter, size, 0644 | IPC_CREAT | IPC_EXCL);
                if(shmid < 0){
                    shmError("shmget");
                }
                //now we need to mount this segment to our adress rack
                model* shmBuffer = (model*)shmat(shmid, NULL, 0);
                if(shmBuffer == NULL){
                    shmError("shmat");
                }
                //now we can write the data we want to write
                copyModel(shmBuffer, &partModel); //something is up with this line
                //and now we pipe the size so that we can read the entire thing from the segment
                if(write(fd[counter][1], &size, sizeof(size_t)) != sizeof(size_t)){
                    pipeError("child", "writing");
                }
                close(fd[counter][WRITE_END]);
                //and at the end we detach the data
                shmdt(shmBuffer);
                freeModel(&partModel);
                exit(EXIT_SUCCESS);
            }
            else{
                forkError("chunk");
            }
        }
    }
    //parent process code
    while((wpid = wait(&status)) > 0){
        //in theory we have to check the WIFEXITED
        if(WEXITSTATUS(status) == EXIT_SUCCESS){
            int childNum = 0;
            for(int i = 0; i < counter; i++){
                if(childrenPids[i] == wpid){
                    childNum = i;
                    break;
                }
            }
            size_t modelSize = -1;
            ssize_t res = read(fd[childNum][READ_END], &modelSize, sizeof(size_t));
            if(res < 0){
                pipeError("parent", "reading");
            }
            close(fd[childNum][READ_END]);
            if(modelSize < 1){
                pipeError("parent", "reading-size value invalid");
            }
            int shmid = shmget(parentId + childNum, modelSize, 0644);
            if(shmid < 0){
                shmError("parent shmget");
            }
            model* shmBuffer = (model*)shmat(shmid, NULL, 0);
            if(shmBuffer == NULL){
                shmError("parent shmat");
            }
            fprintf(stderr, "%s\n", shmBuffer);
            shmdt(shmBuffer);
            //having done what we wanted to do now we can just remove this shared segment
            shmctl(shmid, IPC_RMID, 0);
        }
    }
    
    return EXIT_SUCCESS;
}