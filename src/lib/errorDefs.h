/*!
 @file errorDefs.h
 @brief Contains error definitions for the project
 @details Within this project errors are handled by calling one of these macros. They print an error message to stderr, set errno to a specific value and exit the program with EXIT_FAILURE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/*!
 @defgroup mcErrno Error Reporting
 @brief A shared error reporting system
 @details This shared error reporting system is used to report errors in the project using a global variable mcErrno of type mcError_t
 @see errorDefs, warningDefs on how to work within the system
*/

/*!
 @defgroup errorDefs Error Definitions
 @brief Contains error definitions for the project
 @details These macros are used to report errors in the project
 @ingroup mcErrno
*/

/*!
 @defgroup warningDefs Warning Definitions
 @brief Contains warning definitions for the project
 @details These macros are used to report warnings in the project
 @ingroup mcErrno
*/

/*!
 @enum mcError_t
 @brief Contains error types for the project
 @see mcErrno
 @see errorDefs, warningDefs on how to set these
 @ingroup mcErrno
*/
typedef enum{
    none, /*!< No error, the default value */
    cNBTError, /*!< Error encountered while parsing an NBT file */
    nbtTypeError, /*!< Error when the tag has a different type */
    nbtTagError, /*!< Error when the needed tag is missing */
    fileError, /*!< Error related to C syscalls regarding files */
    parsingError, /*!< Error encountered during the parsing of a file */
    statesWarning, /*!< Warning when a minecraft block state is larger than the palette */
    argError, /*!< Error when an invalid amount of sub-arguments is provided */
    argCountError, /*!< Error when an invalid amount of arguments is provided */
    dirError, /*!< Error when a directory cannot be opened */
    materialWarning, /*!< Warning when the material for a type couldn't be found in the mtl file */
    vertexWarning, /*!< Warning when there is a possible vertex-face mismatch in an object */
    argValError, /*!< Error when an invalid value of an argument is provided */
    pipeError, /*!< Error when something goes wrong with a pipe */
    forkError, /*!< Error when fork fails */
    shmError, /*!< Error when something goes wrong with shared memory */
    localizedFileError, /*!< Error with a specific position in a file */
    semaphoreError, /*!< Error when something goes wrong with a semaphore */
    mallocError, /*!< Error when detecting NULL return value of malloc */
    overflowError, /*!< Error when an overflow condition has been detected */
    stringError /*!< Error when there is an issue with string operations */
} mcError_t;

/*!
 @var mcErrno
 @brief Contains the error type for the project
 @see mcError_t
 @see errorCheck on how to check this
 @see errorDefs, warningDefs on how to set this
 @ingroup mcErrno
*/
extern mcError_t mcErrno;

/*!
 @def raiseError
 @brief Raises an error
 @details Prints the error message, sets errno to EXIT_FAILURE and exits the program
 @param func The name of the function that is being checked
 @ingroup mcErrno
*/
#define raiseError(func) \
    perror(func); \
    exit(EXIT_FAILURE); \
    return EXIT_FAILURE; \

//this return is here to suppress static analysis warnings

/*!
 @def errorCheck
 @brief Checks if an error has been set and exits the program if it has
 @details Mainly a convenience function, call this after performing an operation that might set an error
 @param func The name of the function that is being checked
 @ingroup mcErrno
*/
#define errorCheck(func) \
    if(mcErrno != none || errno != 0){ \
        raiseError(func); \
    }

/*!
 @def clearError
 @brief Clears the error
 @details Sets mcErrno to none and errno to 0
 @ingroup mcErrno
*/
#define clearError() \
    mcErrno = none; \
    errno = 0;

/*!
 @def cNBTError
 @brief Reports an error encountered while parsing an NBT file
 @param filename The name of the file that caused the error
 @ingroup errorDefs
 */
#define cNBTError(filename) \
    mcErrno = cNBTError; \
    fprintf(stderr, "cNBT encountered an error while parsing %s", filename); \

/*!
 @def nbtTypeError
 @brief Reports an error when the tag has a different type
 @param type The current type of the tag
 @param expected The expected type of the tag
 @ingroup errorDefs
 */
#define nbtTypeError(type, expected) \
    mcErrno = nbtTypeError; \
    fprintf(stderr, "Expected type:%d, current type:%d.", expected, type);\

/*!
 @def nbtTagError
 @brief Reports an error when the needed tag is missing
 @param tag The name of the missing tag
 @ingroup errorDefs
 */
#define nbtTagError(tag) \
    mcErrno = nbtTagError; \
    fprintf(stderr, "Tag %s was not found.", tag);\

/*!
 @def fileError
 @brief Reports an error related to C syscalls regarding files
 @param filename The name of the file
 @param action The action being performed on the file
 @ingroup errorDefs
 */
#define fileError(filename, action) \
    mcErrno = fileError; \
    fprintf(stderr, "File %s couldn't be " action ".", filename); \

/*!
 @def parsingError
 @brief Reports an error encountered during the parsing of a file
 @param filename The name of the file being parsed
 @param action The action being performed on the file
 @ingroup errorDefs
 */
#define parsingError(filename, action) \
    mcErrno = parsingError; \
    fprintf(stderr, "Error encountered during " action " of %s.", filename); \

/*!
 @def statesWarning
 @brief Reports a warning when a minecraft block state is larger than the palette
 @param state The current state value
 @param paletteLen The length of the palette
 @param block The name of the block
 @ingroup errorDefs
 */
#define statesWarning(state, paletteLen, block) \
    mcErrno = statesWarning; \
    fprintf(stderr, "%d >= %ld:states warning\n", state, paletteLen); \

/*!
 @def argError
 @brief Reports an error when an invalid amount of sub-arguments is provided
 @param argName The name of the argument
 @param argCount The required number of arguments
 @ingroup errorDefs
 */
#define argError(argName, argCount) \
    mcErrno = argError; \
    fprintf(stderr, "Incorrect number of arguments." argName " requires " argCount " arguments to follow."); \

/*!
 @def argCountError
 @brief Reports an error when an invalid amount of arguments is provided
 @param usage The usage of the program
 @ingroup errorDefs
 */
#define argCountError(usage) \
    mcErrno = argCountError; \
    fprintf(stderr, "Invalid number of arguments was provided\n" usage "\n"); \

/*!
 @def dirError
 @brief Reports an error when a directory cannot be opened
 @param dirname The name of the directory
 @ingroup errorDefs
 */
#define dirError(dirname) \
    mcErrno = dirError; \
    fprintf(stderr, "Directory %s couldn't be opened.\n", dirname); \

/*!
 @def materialWarning
 @brief Reports a warning when the material for a type couldn't be found in the mtl file
 @param type The type of the material
 @ingroup warningDefs
 */
#define materialWarning(type) \
    mcErrno = materialWarning; \
    fprintf(stdout, "Material for %s couldn't be found in the mtl file.\n", type);

/*!
 @def vertexWarning
 @brief Reports a warning when there is a possible vertex-face mismatch in an object
 @param objName The name of the object
 @ingroup warningDefs
 */
#define vertexWarning(objName) \
    mcErrno = vertexWarning; \
    fprintf(stderr, "A possible vertex-face mismatch in object %s\n", objName);

/*!
 @def argValError
 @brief Reports an error when an invalid value of an argument is provided
 @param arg The name of the argument
 @ingroup errorDefs
 */
#define argValError(arg) \
    mcErrno = argValError; \
    fprintf(stderr, "Invalid " arg " argument value.\n"); \

/*!
 @def pipeError
 @brief Reports an error when something goes wrong with a pipe
 @param pipe The name of the pipe
 @param operation The operation being performed on the pipe
 @ingroup errorDefs
 */
#define pipeError(pipe, operation) \
    mcErrno = pipeError; \
    fprintf(stderr, "Pipe " pipe " " operation " failed.\n"); \

/*!
 @def forkError
 @brief Reports an error when fork fails
 @param fork The name of the fork
 @ingroup errorDefs
 */
#define forkError(fork) \
    mcErrno = forkError; \
    fprintf(stderr, "Fork " fork " failed.\n"); \

/*!
 @def shmError
 @brief Reports an error when something goes wrong with shared memory
 @param shm The name of the shared memory
 @ingroup errorDefs
 */
#define shmError(shm) \
    mcErrno = shmError; \
    fprintf(stderr, shm " failed.\n"); \

/*!
 @def localizedFileError
 @brief Reports an error with a specific position in a file
 @param pos The position in the file
 @param fileName The name of the file
 @param action The action being performed on the file
 @extends fileError
 @ingroup errorDefs
 */
#define localizedFileError(pos, fileName, action) \
    fprintf(stderr, "At %lu ", pos); \
    fileError(fileName, action); \
    mcErrno = localizedFileError; 

/*!
 @def semaphoreError
 @brief Reports an error when something goes wrong with a semaphore
 @param sem The name of the semaphore
 @param action The action being performed on the semaphore
 @ingroup errorDefs
 */
#define semaphoreError(sem, action) \
    mcErrno = semaphoreError; \
    fprintf(stderr, "Semaphore " sem " couldn't " action "\n"); \

/*!
 @def mallocError
 @brief Reports an error when detecting NULL return value of malloc
 @param malloc The name of the malloc
 @param size The size of the memory being allocated
 @extends perror
 @ingroup errorDefs
 */
#define mallocError(malloc, size) \
    mcErrno = mallocError; \
    fprintf(stderr, "Malloc " malloc " of size %zu couldn't be performed. Check for uninitialized values.\n", size); \
    if(errno == 0){errno = ENOMEM;} \
    perror("Malloc error"); \

/*!
 @def overflowError
 @brief Reports an error when an overflow condition has been detected
 @param type The type of the overflow
 @extends EOVERFLOW
 @ingroup errorDefs
 */
#define overflowError(type) \
    mcErrno = overflowError; \
    fprintf(stderr, "An overflow condition has been detected of " type "."); \
    errno = EOVERFLOW; \

/*!
 @def stringError
 @brief Reports an error when there is an issue with string operations
 @ingroup errorDefs
 */
#define stringError(function) \
    mcErrno = stringError; \
    fprintf(stderr, "Function " function " encountered a problem.\n"); \
        
