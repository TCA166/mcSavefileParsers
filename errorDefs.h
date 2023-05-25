//Header file for error handling macros

#define nbtFileError(filename) \
        fprintf(stderr, "File %s couldn't be located.", filename); \
        perror("File error"); \
        exit(-1); \

#define nbtTypeError(type, expected) \
        fprintf(stderr, "Expected type:%d, current type:%d.", expected, type);\
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

#define nbtTagError(tag) \
        fprintf(stderr, "Tag %s was not found.", tag);\
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

#define fileError(filename) \
        fprintf(stderr, "File %s couldn't be located.", filename); \
        perror("File error.");\
        exit(EXIT_FAILURE);

#define statesError(state, paletteLen, block) \
        fprintf(stderr, "%d >= %d:states error\n", state, paletteLen); \
        block.type = NULL;

#define argError(argName, argCount) \
        fprintf(stderr, "Incorrect number of arguments." argName " requires " argCount " arguments to follow."); \
        break;
