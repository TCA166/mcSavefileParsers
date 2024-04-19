#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "errorDefs.h"

#ifdef WIN32
#define directorySeperator "\\"
#else
#define directorySeperator "/"
#endif

int main(int argc, char** argv){
    char s = 0; //if we want to extract a simplified mtl
    if(argc < 2){
        argCountError();
    }
    for(int i = 3; i < argc; i++){
        if(strcmp(argv[i], "-s") == 0){
            s = 1;
        }
    }
    //open the directory
    DIR *d = opendir(".");
    struct dirent *dirent; //currently analysed directory entity 
    if(d == NULL){
        dirError(argv[1]);
    }
    while ((dirent = readdir(d)) != NULL){
        char* extension = strchr(dirent->d_name, '.');
        if(extension != NULL){
            extension++;
            if(strcmp(extension, "png") == 0){
                char* fullname = malloc(strlen(argv[1]) + strlen(dirent->d_name) + 2);
                strcat(fullname, argv[1]);
                strcat(fullname, directorySeperator);
                strcat(fullname, dirent->d_name);
                if(s){
                    FILE* fp = fopen(fullname, "rb");
                    fseek(fp, 0, SEEK_END);
                    long sz = ftell(fp);
                    unsigned char* bytes = malloc(sz);
                    fread(bytes, sz, 1, fp);
                    //Check PNG header
                    if(bytes[0] == 137 && bytes[1] == 80 && bytes[2] == 78 && bytes[3] == 71 && bytes[4] == 13 && bytes[5] == 10 && bytes[6] == 26 && bytes[7] == 10){
                        
                    }
                    fclose(fp);
                }
                
                free(fullname);
            }
        }
    }
    closedir(d);
    
}