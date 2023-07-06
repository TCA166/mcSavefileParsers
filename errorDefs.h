//Header file for error handling macros

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
        fprintf(stderr, "No arguments were provided"); \
        errno = EINVAL; \
        perror("Arg count error."); \
        exit(EXIT_FAILURE);

#define dirError(dirname) \
        fprintf(stderr, "Directory %s couldn't be opened", dirname); \
        errno = ENOENT; \
        perror("Dir error."); \
        exit(EXIT_FAILURE);

#define materialWarning(type) \
        fprintf(stderr, "Material for %s couldn't be found in the mtl file.\n", type);

#define vertexWarning(objName) \
        fprintf(stderr, "A possible vertex-face mismatch in object %s\n", objName);
