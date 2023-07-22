#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "errorDefs.h"
#include "generator.h"
#include "regionParser.h"

#if defined(__unix__)
    //All of these are POSIX compliant according to this https://pubs.opengroup.org/onlinepubs/9699919799/idx/head.html
    #include <sys/types.h> //for types in unistd
    #include <sys/wait.h> //for wait
    #include <sys/mman.h> //for mmap
    #include <semaphore.h> //POSIX semaphores
    #include <fcntl.h> //(flag control) for macros like O_CREAT
    #include <sys/shm.h>
    #include <unistd.h> //for fork
    //These two macros provide a nice level of abstraction and clearly show what we want to do with this mmap
    #define sharedMalloc(size) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)
    #define sharedFree(ptr, size) munmap(ptr, size);
    #define WRITE_END 1
    #define READ_END 0
#elif defined(_WIN32)
    //We are on windows 32bit or 64 bit
    #include <sys/types.h> //for types in unistd
    #include <semaphore.h> //POSIX semaphores
    #include <fcntl.h> //(flag control) for macros like O_CREAT
    #include <windows.h>

    #define semAction(action) \
        { \
            bool cont = true; \
            while(cont){ \
                DWORD dwWaitResult = WaitForSingleObject(ghSemaphore, 0); \
                switch(dwWaitResult){ \
                    case WAIT_OBJECT_0:; \
                        cont = false; \
                        action \
                        ReleaseSemaphore(ghSemaphore, 1, NULL); \
                        break; \
                    case WAIT_TIMEOUT:; \
                        break; \
                } \
            } \
        }

    //For whatever reason multiprocessing on Windows is... discouraged 

    HANDLE ghSemaphore;

    //struct for passing parameters to a thread. I tell you windows approach to multiprocessing and threading is superior
    struct threadParams{
        char** output;
        int x;
        int z;
        char* regionDirPath;
        hashTable* materials;
        hashTable* objects;
        bool yLim;
        int upLim;
        int downLim;
        int side;
        unsigned long* offset;
        unsigned int* index;
    };

    //Function that the threads will do
    DWORD WINAPI ThreadProc( LPVOID lpParam ){
        struct threadParams* params = (struct threadParams*)lpParam;
        chunk ourChunk;
        semAction(
            ourChunk = extractChunk(params->regionDirPath, params->x, params->z);
        )
        model m = generateFromNbt(ourChunk.data, ourChunk.byteLength, params->materials, params->objects, params->yLim, params->upLim, params->downLim, true, false, params->side, params->x, params->z);
        unsigned long diff = getTotalVertexCount(m);
        unsigned long localOffset = 0;
        unsigned int localIndex = 0;
        //Windows way of doing a semaphore check. Superior I tell you
        semAction(
            localOffset = *(params->offset);
            localIndex = *(params->index);
            *(params->offset) += diff;
            *(params->index) += 1;
        )
        size_t sz = 0;
        char* modelStr = generateModel(&m, &sz, NULL, &localOffset);
        freeModel(&m);
        params->output[localIndex] = calloc(sz, 1);
        strcpy(params->output[localIndex], modelStr);
        free(modelStr);
        free(params);
        return EXIT_SUCCESS;
    }
#else
    #error "Not being compiled on POSIX compliant system"
#endif

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
    if(radius < 0){
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

    //matches the counter after the double for loop has run it's course
    int numChildren = ((xCenter + radius) - (xCenter - radius) + 1) * ((zCenter + radius) - (zCenter - radius) + 1); 

    //Main code is platform dependant. As such there is this arguably cursed preprocessor insert
    #if defined(__unix__)
    /*HOW THIS WORKS
    Foreach chunk in the given radius we create a separate process.
    This process does what modelGenerator would do with some differences. 
    Most notably it reads a shared variable called offset between processes that informs it of at which vertex offset it should generate.
    This variable is protected by a semaphor and is logged so that the complete model can be pieced together in the right order from chunks.
    After generating the model the child process creates a shared memory segment, outputs the model part into it and then pipes the size of the segment to parent.
    Then parent recieves all the pieces and puts together the final model
    */

    //Ok so we are going to store the current vertex offset in a shared buffer between processes
    unsigned long* offset = (unsigned long*)sharedMalloc(sizeof(unsigned long));
    if(offset != MAP_FAILED){
        *offset = 1; //current vertex offset
    }
    else{
        shmError("offset mmap");
    }
    //Unnamed shared semaphore
    sem_t *sem = sharedMalloc(sizeof(sem_t));
    if(sem == MAP_FAILED){
        shmError("semaphore map");
    }
    if(sem_init(sem, 1, 1) < 0){
        semaphoreError("main", "be initialized");
    }
    //and now the big thing
    pid_t parentId = getpid();
    //shared array that will contain the assembly order of the finished model
    short* order = (short*)sharedMalloc(sizeof(short) * numChildren); 
    int* index = (int*)sharedMalloc(sizeof(int));
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
                close(STDOUT_FILENO); //we close STDOUT to suppress output of generateModel
                model partModel = generateFromNbt(ourChunk.data, ourChunk.byteLength, materials, objects, yLim, upLim, downLim, true, false, side, ourChunk.x, ourChunk.z);
                unsigned long diff = getTotalVertexCount(partModel);
                sem_wait(sem); /*CRITICAL SECTION*/
                unsigned long localOffset = *offset;
                *offset += diff;
                order[*index] = counter;
                (*index)++;
                sem_post(sem); /*END OF*/
                sem_close(sem);
                size_t size = 0;
                char* modelStr = generateModel(&partModel, &size, NULL, &localOffset);
                freeModel(&partModel);
                //fprintf(stderr, "%p\n", materials);
                //Ok so we have the string, now we have to transfer it over.
                //Unfortunately pipes have an upper limit. One that can be easily reached with our strings
                //So we are creating a shared memory buffer for that now :) We have gone full circle
                int shmid = shmget(parentId + counter, size, 0644 | IPC_CREAT | IPC_EXCL);
                if(shmid < 0){
                    shmError("shmget");
                }
                char* shmBuffer = (char*)shmat(shmid, NULL, 0);
                if(shmBuffer == NULL){
                    shmError("shmat");
                }
                //Ok so we created the shared buffer now we can write to it
                strcpy(shmBuffer, modelStr);
                //And then we pipe over the size so that the other side can prepare itself for reading
                if(write(fd[counter][WRITE_END], &size, sizeof(size_t)) != sizeof(size_t)){
                    pipeError("child", "writing size");
                }
                close(fd[counter][WRITE_END]);
                free(modelStr);
                exit(EXIT_SUCCESS);
            }
            else{
                forkError("chunk");
            }
        }
    }
    sem_destroy(sem);
    sharedFree(sem, sizeof(sem_t));
    int progress = 0;
    size_t currentSize = 1;
    char** parts = calloc(numChildren, sizeof(char*));
    int finished = 0;
    //parent process code
    while((wpid = wait(&status)) > 0){
        progress++;
        fprintf(stdout, "%.2f%% done\r", ((float)progress / (float)numChildren) * 100);
        fflush(stdout);
        //in theory we have to check the WIFEXITED
        if(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS){
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
            close(fd[childNum][READ_END]);
            int shmid = shmget(parentId + childNum, size, 0644);
            if(shmid < 0){
                shmError("parent shmget");
            }
            char* shmBuffer = (char*)shmat(shmid, NULL, 0);
            if(shmBuffer == NULL){
                shmError("parent shmat");
            }
            strcpy(parts[childNum], shmBuffer);
            shmdt(shmBuffer);
            //having done what we wanted to do now we can just remove this shared segment
            shmctl(shmid, IPC_RMID, 0);
            currentSize += size;
        }
        else{
            fprintf(stderr, "Process %d failed.\n", wpid);
        }
    }
    printf("Model parts generated\n");
    free(childrenPids);
    free(fd);
    sharedFree(offset, sizeof(unsigned long));
    char* result = malloc(currentSize);
    result[0] = '\0';
    if(materialFilename != NULL){
        appendMtlLine(materialFilename, result, &currentSize);
    }
    for(int i = 0; i < *index; i++){
        if(parts[order[i]] != NULL){
            strcat(result, parts[order[i]]);
        }
        free(parts[order[i]]);
    }
    free(parts);
    sharedFree(index, sizeof(int));
    sharedFree(order, sizeof(short) * numChildren);
    #elif defined(_WIN32)
    //Windows code: multithreaded version of the unix code
    ghSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
    unsigned long* offset = malloc(sizeof(unsigned long));
    *offset = 1;
    unsigned int* index = malloc(sizeof(unsigned int));
    *index = 0;
    char** modelStrs = calloc(numChildren, sizeof(char*));
    int counter = 0;
    int stdout_copy = dup(STDOUT_FILENO);
    printf("Starting model parts generation...\n");
    close(STDOUT_FILENO);
    HANDLE* threads = calloc(numChildren, sizeof(HANDLE));
    for(int x = xCenter - radius; x <= xCenter + radius; x++){
        for(int z = zCenter - radius; z <= zCenter + radius; z++){
            struct threadParams* inParams = malloc(sizeof(struct threadParams));
            inParams->x = x;
            inParams->z = z;
            inParams->regionDirPath = regionDirPath;
            inParams->output = modelStrs;
            inParams->downLim = downLim;
            inParams->materials = materials;
            inParams->objects = objects;
            inParams->offset = offset;
            inParams->index = index;
            inParams->side = side;
            inParams->upLim = upLim;
            inParams->yLim = yLim;
            threads[counter] = CreateThread(NULL, 0, ThreadProc, inParams, 0, NULL);
            counter++;
        }
    }
    WaitForMultipleObjects(numChildren, threads, TRUE, INFINITE);
    dup2(stdout_copy, STDOUT_FILENO);
    close(stdout_copy);
    printf("Threads finished\n");
    fflush(stdout);
    size_t currentSize = 1;
    char* result = malloc(currentSize);
    result[0] = '\0';
    if(materialFilename != NULL){
        appendMtlLine(materialFilename, result, &currentSize);
    }
    for(int i = 0; i < numChildren; i++){
        CloseHandle(threads[i]);
        currentSize += strlen(modelStrs[i]) + 1;
        result = realloc(result, currentSize);
        strcat(result, modelStrs[i]);
        free(modelStrs[i]);
    }
    CloseHandle(ghSemaphore);
    #endif
    FILE* outFile = fopen(outFilename, "w");
    fwrite(result, currentSize, 1, outFile);
    fclose(outFile);
    free(result);
    return EXIT_SUCCESS;
}