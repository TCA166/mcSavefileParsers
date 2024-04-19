/*!
 * @file errorDefs.h
 * @brief Contains error definitions for the project
 * 
 * @details Within this project errors are handled by calling one of these macros. They print an error message to stderr, set errno to a specific value and exit the program with EXIT_FAILURE.
 */

//Since the ERRNO macros are very unspecific and as far as I can see usually associated with errors a kernel may encounter, I have only loosely associated them with my own error convention

/*!
 @defgroup errorDefs Error Definitions
 @brief Contains error definitions for the project
*/

/*!
 @defgroup warningDefs Warning Definitions
 @brief Contains warning definitions for the project
*/

/*!
 * @def cNBTError
 * @brief Reports an error encountered while parsing an NBT file
 * @param filename The name of the file that caused the error
 * @ingroup errorDefs
 */
#define cNBTError(filename) \
        fprintf(stderr, "cNBT encountered an error while parsing %s", filename); \
        perror("cNBT error"); \
        exit(EXIT_FAILURE); \

/*!
 * @def nbtTypeError
 * @brief Reports an error when the tag has a different type
 * @param type The current type of the tag
 * @param expected The expected type of the tag
 * @ingroup errorDefs
 */
#define nbtTypeError(type, expected) \
        fprintf(stderr, "Expected type:%d, current type:%d.", expected, type);\
        errno = EIO; \
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

/*!
 * @def nbtTagError
 * @brief Reports an error when the needed tag is missing
 * @param tag The name of the missing tag
 * @ingroup errorDefs
 */
#define nbtTagError(tag) \
        fprintf(stderr, "Tag %s was not found.", tag);\
        errno = EIO; \
        perror("Nbt type error"); \
        exit(EXIT_FAILURE);

/*!
 * @def fileError
 * @brief Reports an error related to C syscalls regarding files
 * @param filename The name of the file
 * @param action The action being performed on the file
 * @ingroup errorDefs
 */
#define fileError(filename, action) \
        fprintf(stderr, "File %s couldn't be " action ".", filename); \
        errno = EIO; \
        perror("File error.");\
        exit(EXIT_FAILURE);

/*!
 * @def parsingError
 * @brief Reports an error encountered during the parsing of a file
 * @param filename The name of the file being parsed
 * @param action The action being performed on the file
 * @ingroup errorDefs
 */
#define parsingError(filename, action) \
        fprintf(stderr, "Error encountered during " action " of %s.", filename); \
        errno = EIO; \
        perror("Parsing error.");\
        exit(EXIT_FAILURE);

/*!
 * @def statesError
 * @brief Reports a warning when a minecraft block state is larger than the palette
 * @param state The current state value
 * @param paletteLen The length of the palette
 * @param block The name of the block
 * @ingroup errorDefs
 */
#define statesError(state, paletteLen, block) \
        fprintf(stderr, "%d >= %ld:states error\n", state, paletteLen); \

/*!
 * @def argError
 * @brief Reports an error when an invalid amount of sub-arguments is provided
 * @param argName The name of the argument
 * @param argCount The required number of arguments
 * @ingroup errorDefs
 */
#define argError(argName, argCount) \
        fprintf(stderr, "Incorrect number of arguments." argName " requires " argCount " arguments to follow."); \
        errno = EINVAL; \
        perror("Arg error"); \
        break;

/*!
 * @def argCountError
 * @brief Reports an error when an invalid amount of arguments is provided
 * @ingroup errorDefs
 */
#define argCountError() \
        fprintf(stderr, "Invalid number of arguments was provided\n."); \
        errno = EINVAL; \
        perror("Arg count error."); \
        exit(EXIT_FAILURE);

/*!
 * @def dirError
 * @brief Reports an error when a directory cannot be opened
 * @param dirname The name of the directory
 * @ingroup errorDefs
 */
#define dirError(dirname) \
        fprintf(stderr, "Directory %s couldn't be opened.\n", dirname); \
        errno = ENOENT; \
        perror("Dir error."); \
        exit(EXIT_FAILURE);

/*!
 * @def materialWarning
 * @brief Reports a warning when the material for a type couldn't be found in the mtl file
 * @param type The type of the material
 * @ingroup warningDefs
 */
#define materialWarning(type) \
        fprintf(stdout, "Material for %s couldn't be found in the mtl file.\n", type);

/*!
 * @def vertexWarning
 * @brief Reports a warning when there is a possible vertex-face mismatch in an object
 * @param objName The name of the object
 * @ingroup warningDefs
 */
#define vertexWarning(objName) \
        fprintf(stderr, "A possible vertex-face mismatch in object %s\n", objName);

/*!
 * @def argValError
 * @brief Reports an error when an invalid value of an argument is provided
 * @param arg The name of the argument
 * @ingroup errorDefs
 */
#define argValError(arg) \
        fprintf(stderr, "Invalid " arg " argument value.\n"); \
        errno = EINVAL; \
        perror("Arg val error"); \
        exit(EXIT_FAILURE); 

/*!
 * @def pipeError
 * @brief Reports an error when something goes wrong with a pipe
 * @param pipe The name of the pipe
 * @param operation The operation being performed on the pipe
 * @ingroup errorDefs
 */
#define pipeError(pipe, operation) \
        fprintf(stderr, "Pipe " pipe " " operation " failed.\n"); \
        errno = EPIPE; \
        perror("Pipe error"); \
        exit(EXIT_FAILURE); 

/*!
 * @def forkError
 * @brief Reports an error when fork fails
 * @param fork The name of the fork
 * @ingroup errorDefs
 */
#define forkError(fork) \
        fprintf(stderr, "Fork " fork " failed.\n"); \
        errno = ECHILD; \
        perror("Pipe error"); \
        exit(EXIT_FAILURE);

/*!
 * @def shmError
 * @brief Reports an error when something goes wrong with shared memory
 * @param shm The name of the shared memory
 * @ingroup errorDefs
 */
#define shmError(shm) \
        fprintf(stderr, shm " failed.\n"); \
        perror("Shared memory error"); \
        exit(EXIT_FAILURE);

/*!
 * @def localizedFileError
 * @brief Reports an error with a specific position in a file
 * @param pos The position in the file
 * @param fileName The name of the file
 * @param action The action being performed on the file
 * @ingroup errorDefs
 */
#define localizedFileError(pos, fileName, action) \
        fprintf(stderr, "At %lu ", pos); \
        fileError(fileName, action);

/*!
 * @def semaphoreError
 * @brief Reports an error when something goes wrong with a semaphore
 * @param sem The name of the semaphore
 * @param action The action being performed on the semaphore
 * @ingroup errorDefs
 */
#define semaphoreError(sem, action) \
        fprintf(stderr, "Semaphore " sem " couldn't " action "\n"); \
        perror("Semaphore error"); \
        exit(EXIT_FAILURE);

/*!
 * @def mallocError
 * @brief Reports an error when detecting NULL return value of malloc
 * @param malloc The name of the malloc
 * @param size The size of the memory being allocated
 * @ingroup errorDefs
 */
#define mallocError(malloc, size) \
        fprintf(stderr, "Malloc " malloc " of size %zu couldn't be performed. Check for uninitialized values.\n", size); \
        if(errno == 0){errno = ENOMEM;} \
        perror("Malloc error"); \
        exit(EXIT_FAILURE);

/*!
 * @def overflowError
 * @brief Reports an error when an overflow condition has been detected
 * @param type The type of the overflow
 * @ingroup errorDefs
 */
#define overflowError(type) \
        fprintf(stderr, "An overflow condition has been detected of " type ". To prevent a crash further down the line program will be shut down."); \
        errno = EOVERFLOW; \
        perror("Overflow error"); \
        exit(EXIT_FAILURE);

/*!
 * @def stringError
 * @brief Reports an error when there is an issue with string operations
 * @ingroup errorDefs
 */
#define stringError(function) \
        fprintf(stderr, "Function " function " encountered a problem.\n"); \
        perror("String function error"); \
        exit(EXIT_FAILURE);
