#include "errorDefs.h"
#include <errno.h>

mcError_t mcErrno;

void errorCheck(const char* func){
    if(mcErrno != none || errno != 0){
        perror(func);
        exit(EXIT_FAILURE);
    }
}

void clearError(){
    mcErrno = none;
    errno = 0;
}
