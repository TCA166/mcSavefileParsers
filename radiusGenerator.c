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
#include <sys/types.h> //for types in unistd
#include <sys/wait.h> //for wait
#include <unistd.h> //for fork
#include <sys/mman.h> //for mmap
#include <semaphore.h> //POSIX semaphores
#include <fcntl.h> //(flag control) for macros like O_CREAT

#define WRITE_END 1
#define READ_END 0

#define SNAME "/offsetSem"

#define protection PROT_READ | PROT_WRITE
#define visibility MAP_SHARED | MAP_ANONYMOUS

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
    //Ok so we are going to store the current vertex offset in a shared buffer between processes
    unsigned long* offset = (unsigned long*)mmap(NULL, sizeof(unsigned long), protection, visibility, -1, 0);
    if(offset != MAP_FAILED){
        *offset = 1; //current vertex offset
    }
    else{
        shmError("offset mmap");
    }
    sem_t *sem = sem_open(SNAME, O_CREAT, 0644, 3);
    //and now the big thing

    //matches the counter after the double for loop has run it's course
    int numChildren = ((xCenter + radius) - (xCenter - radius) + 1) * ((zCenter + radius) - (zCenter - radius) + 1); 
    //shared array that will contain the assembly order of the finished model
    short* order = (short*)mmap(NULL, sizeof(short) * numChildren, protection, visibility, -1, 0); 
    int* index = (int*)mmap(NULL, sizeof(int), protection, visibility, -1, 0);
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
                //ok so the idea with the shared memory segment failed due to the inability of deep copying simply the model into such a segment (pointers will be invalid)
                sem_wait(sem); /*CRITICAL SECTION*/
                unsigned long localOffset = *offset;
                int diff = 0;
                //ok so we need to now calculate by how much we want to increase the offset
                foreachObject((&partModel)){
                    struct object* object = partModel.objects[x][y][z];
                    diff += object->vertexCount;
                }
                *offset += diff;
                order[*index] = counter;
                sem_post(sem); /*END OF*/
                close(STDOUT_FILENO); //we close STDOUT to suppress output of generateModel
                size_t size = 0;
                char* modelStr = generateModel(&partModel, &size, NULL, &localOffset);
                if(write(fd[counter][WRITE_END], &size, sizeof(size_t)) != sizeof(size_t)){
                    pipeError("child", "writing size");
                }
                if(write(fd[counter][WRITE_END], modelStr, size) != size){ //all children just hang on this line. No clue as to why
                    pipeError("child", "writing modelStr");
                }
                fprintf(stderr, "test");
                close(fd[counter][WRITE_END]);
                freeModel(&partModel);
                free(modelStr);
                exit(EXIT_SUCCESS);
            }
            else{
                forkError("chunk");
            }
        }
    }
    size_t currentSize = 1;
    char** parts = calloc(numChildren, sizeof(char*));
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
            size_t size = -1;
            ssize_t res = read(fd[childNum][READ_END], &size, sizeof(size_t));
            if(res < 0){
                pipeError("parent", "reading 1");
            }
            if(size < 1){
                pipeError("parent", "reading-size value invalid");
            }
            parts[childNum] = malloc(size);
            res = read(fd[childNum][READ_END], parts[childNum], size);
            if(res < size){
                pipeError("parent", "reading 2");
            }
            close(fd[childNum][READ_END]);
            currentSize += size;
        }
    }
    printf("Model parts generated\n");
    free(childrenPids);
    free(fd);
    munmap(offset, sizeof(unsigned long));
    char* result = malloc(currentSize);
    result[0] = '\0';
    for(int i = 0; i < *index; i++){
        strcat(result, parts[order[i]]);
        free(parts[order[i]]);
    }
    free(parts);
    munmap(index, sizeof(int));
    munmap(order, sizeof(short) * numChildren);
    FILE* outFile = fopen(outFilename, "w");
    fwrite(result, currentSize, 1, outFile);
    fclose(outFile);
    return EXIT_SUCCESS;
}