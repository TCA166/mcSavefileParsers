//Header file for error handling macros

//Since the ERRNO macros are very unspecific and as far as I can see usually associated with errors a kernel may encounter, I have only loosely associated them with my own error convention

//When cNBT itself reports an error
#define cNBTError(filename) \
        fprintf(stderr, "cNBT encountered an error while parsing %s", filename); \
        perror("cNBT error"); \
        exit(EXIT_FAILURE); \

//When cNBT reports the tag has a different type
#define nbtTypeError(type, expected) \
        fprintf(stderr, "Expected type:%d, current type:%d.", expected, type);\
        errno = EIO; \
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

//When cNBT reports the needed tag is missing
#define nbtTagError(tag) \
        fprintf(stderr, "Tag %s was not found.", tag);\
        errno = EIO; \
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

//When anything goes wrong with C syscalls regarding files
#define fileError(filename, action) \
        fprintf(stderr, "File %s couldn't be " action ".", filename); \
        errno = EIO; \
        perror("File error.");\
        exit(EXIT_FAILURE);

//When anything goes wrong during the parsing of a file
#define parsingError(filename, action) \
        fprintf(stderr, "Error encountered during " action " of %s.", filename); \
        errno = EIO; \
        perror("Parsing error.");\
        exit(EXIT_FAILURE);

//More of a warning really since it's non critical
//When a minecraft block state is larger than the palette
#define statesError(state, paletteLen, block) \
        fprintf(stderr, "%d >= %d:states error\n", state, paletteLen); \

//When user provides an invalid amount of sub-arguments
#define argError(argName, argCount) \
        fprintf(stderr, "Incorrect number of arguments." argName " requires " argCount " arguments to follow."); \
        errno = EINVAL; \
        perror("Arg error"); \
        break;

//When user provides an invalid amount of arguments
#define argCountError() \
        fprintf(stderr, "Invalid number of arguments was provided\n."); \
        errno = EINVAL; \
        perror("Arg count error."); \
        exit(EXIT_FAILURE);

//When directory cannot be opened
#define dirError(dirname) \
        fprintf(stderr, "Directory %s couldn't be opened.\n", dirname); \
        errno = ENOENT; \
        perror("Dir error."); \
        exit(EXIT_FAILURE);

#define materialWarning(type) \
        fprintf(stdout, "Material for %s couldn't be found in the mtl file.\n", type);

#define vertexWarning(objName) \
        fprintf(stderr, "A possible vertex-face mismatch in object %s\n", objName);

//When user provides an invalid value of an argument
#define argValError(arg) \
        fprintf(stderr, "Invalid " arg " argument value.\n"); \
        errno = EINVAL; \
        perror("Arg val error"); \
        exit(EXIT_FAILURE); 

//When something goes wrong with a pipe
#define pipeError(pipe, operation) \
        fprintf(stderr, "Pipe " pipe " " operation " failed.\n"); \
        errno = EPIPE; \
        perror("Pipe error"); \
        exit(EXIT_FAILURE); 

//When fork fails
#define forkError(fork) \
        fprintf(stderr, "Fork " fork " failed.\n"); \
        errno = ECHILD; \
        perror("Pipe error"); \
        exit(EXIT_FAILURE);

//When anything goes wrong with shared memory
#define shmError(shm) \
        fprintf(stderr, shm " failed.\n"); \
        perror("Shared memory error"); \
        exit(EXIT_FAILURE);

//When you want to provide a place in file where an error took place
#define localizedFileError(pos, fileName, action) \
        fprintf(stderr, "At %lu ", pos); \
        fileError(fileName, action);

//When anything goes wrong with a semaphore
#define semaphoreError(sem, action) \
        fprintf(stderr, "Semaphore " sem " couldn't " action "\n"); \
        perror("Semaphore error"); \
        exit(EXIT_FAILURE);

//Used when detecting NULL return value of malloc
#define mallocError(malloc, size) \
        fprintf(stderr, "Malloc " malloc " of size %zu couldn't be performed. Check for uninitialized values.\n", size); \
        if(errno == 0){errno = ENOMEM;} \
        perror("Malloc error"); \
        exit(EXIT_FAILURE);

#define overflowError(type) \
        fprintf(stderr, "An overflow condition has been detected of " type ". To prevent a crash further down the line program will be shut down."); \
        errno = EOVERFLOW; \
        perror("Overflow error"); \
        exit(EXIT_FAILURE);
        