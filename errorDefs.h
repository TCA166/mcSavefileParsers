//Header file for error handling macros

//Since the ERRNO macros are very unspecific and as far as I can see usually associated with errors a kernel may encounter, I have only loosely associated them with my own error convention

#define nbtFileError(filename) \
        fprintf(stderr, "File %s couldn't be located.", filename); \
        errno = ENOENT; \
        perror("File error"); \
        exit(EXIT_FAILURE); \

#define nbtTypeError(type, expected) \
        fprintf(stderr, "Expected type:%d, current type:%d.", expected, type);\
        errno = EIO; \
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

#define nbtTagError(tag) \
        fprintf(stderr, "Tag %s was not found.", tag);\
        errno = EIO; \
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

#define fileError(filename, action) \
        fprintf(stderr, "File %s couldn't be " action ".", filename); \
        errno = EIO; \
        perror("File error.");\
        exit(EXIT_FAILURE);

#define parsingError(filename, action) \
        fprintf(stderr, "Error encountered during " action " of %s.", filename); \
        errno = EIO; \
        perror("Parsing error.");\
        exit(EXIT_FAILURE);

#define statesError(state, paletteLen, block) \
        fprintf(stderr, "%d >= %d:states error\n", state, paletteLen); \
        block.type = NULL;

#define argError(argName, argCount) \
        fprintf(stderr, "Incorrect number of arguments." argName " requires " argCount " arguments to follow."); \
        break;

#define argCountError() \
        fprintf(stderr, "Invalid number of arguments was provided\n."); \
        errno = EINVAL; \
        perror("Arg count error."); \
        exit(EXIT_FAILURE);

#define dirError(dirname) \
        fprintf(stderr, "Directory %s couldn't be opened.\n", dirname); \
        errno = ENOENT; \
        perror("Dir error."); \
        exit(EXIT_FAILURE);

#define materialWarning(type) \
        fprintf(stderr, "Material for %s couldn't be found in the mtl file.\n", type);

#define vertexWarning(objName) \
        fprintf(stderr, "A possible vertex-face mismatch in object %s\n", objName);

#define argValError(arg) \
        fprintf(stderr, "Invalid " arg " argument value.\n"); \
        errno = EINVAL; \
        perror("Arg val error"); \
        exit(EXIT_FAILURE); 

#define pipeError(pipe, operation) \
        fprintf(stderr, "Pipe " pipe " " operation " failed.\n"); \
        errno = EPIPE; \
        perror("Pipe error"); \
        exit(EXIT_FAILURE); 

#define forkError(fork) \
        fprintf(stderr, "Fork " fork " failed.\n"); \
        errno = ECHILD; \
        perror("Pipe error"); \
        exit(EXIT_FAILURE);

#define shmError(shm) \
        fprintf(stderr, shm " failed.\n"); \
        perror("Shared memory error"); \
        exit(EXIT_FAILURE);
