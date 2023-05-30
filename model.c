#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "model.h"

#define freeCubeFace(c, n) free(c.faces[n]); c.faces[n] = NULL;

#define faceCheck(t) strcmp(t.type, ignoreType) != 0 && (t.m == NULL || t.m->d == 1)

int digits(int i){
    if(i == 0){
        return 1;
    }
    return floor(log10(abs(i))) + 1;
}

model initModel(int x, int y, int z){
    model newModel;
    newModel.x = x;
    newModel.y = y;
    newModel.z = z;
    newModel.cubes = malloc(x * sizeof(struct cube**));
    for(int i = 0; i < x; i++){
        newModel.cubes[i] = malloc(y * sizeof(struct cube*));
        for(int n = 0; n < y; n++){
            newModel.cubes[i][n] = malloc(z * sizeof(struct cube));
        }
    }
    return newModel;
}

struct vertex newVertex(int x, int y, int z){
    struct vertex new;
    new.x = x;
    new.y = y;
    new.z = z;
    return new;
}

//For whatever reason this needs to be true
//a > b && c < d
struct cubeFace* newCubeFace(int a, int b, int c, int d){
    struct cubeFace* face = malloc(sizeof(struct cubeFace));
    face->v1 = a;
    face->v2 = b;
    face->v3 = c;
    face->v4 = d;
    return face;
}

struct face deCube(struct cubeFace face, struct cube originalCube){
    struct face new;
    new.vertices = malloc(sizeof(struct vertex) * 4);
    new.vertices[0] = originalCube.vertices[face.v1];
    new.vertices[1] = originalCube.vertices[face.v2];
    new.vertices[2] = originalCube.vertices[face.v3];
    new.vertices[3] = originalCube.vertices[face.v4];
    return new;
}

void cullFaces(model* thisModel, char cullChunkBorder, char* ignoreType, struct material* materials, int materialLen){
    for(int x = 0; x < thisModel->x; x++){
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct cube c = thisModel->cubes[x][y][z];
                //x
                if( x + 1 >= thisModel->x){
                    if(cullChunkBorder){
                        freeCubeFace(c, 0);
                    }
                }
                else if(faceCheck(thisModel->cubes[x + 1][y][z])){
                    freeCubeFace(c, 0);
                }
                if( x - 1 < 0){
                    if(cullChunkBorder){
                        freeCubeFace(c, 4);
                    }
                }
                else if(faceCheck(thisModel->cubes[x - 1][y][z])){
                    freeCubeFace(c, 4);
                }
                //y
                if( y + 1 >= thisModel->y){
                    if(cullChunkBorder){
                        freeCubeFace(c, 1);
                    }
                }
                else if(faceCheck(thisModel->cubes[x][y + 1][z])){
                    freeCubeFace(c, 1);
                }
                if( y - 1 < 0){
                    if(cullChunkBorder){
                        freeCubeFace(c, 5);
                    }
                }
                else if(faceCheck(thisModel->cubes[x][y - 1][z])){
                    freeCubeFace(c, 5);
                }
                //z
                if( z + 1 >= thisModel->z){
                    if(cullChunkBorder){
                        freeCubeFace(c, 2);
                    }
                }
                else if(faceCheck(thisModel->cubes[x][y][z + 1])){
                    freeCubeFace(c, 2);
                }
                if( z - 1 < 0){
                    if(cullChunkBorder){
                        freeCubeFace(c, 3);
                    }
                }
                else if(faceCheck(thisModel->cubes[x][y][z - 1])){
                    freeCubeFace(c, 3);
                }
                thisModel->cubes[x][y][z] = c;
            }
        }
    }
}

unsigned char isNotEmpty(struct cube c){
    for(int i = 0; i < 6; i++){
        if(c.faces[i] != NULL){
            return 1;
        }
    }
    return 0;
}

char* generateModel(model* thisModel, size_t* outSize, char* ignoreType, char* materialFileName){
    char* fileContents = NULL;
    (*outSize)++;
    fileContents = malloc(*outSize);
    fileContents[0] = '\0';
    if(materialFileName != NULL){
        *outSize += 9 + strlen(materialFileName);
        fileContents = realloc(fileContents, *outSize);
        strcat(fileContents, "mtllib ");
        strcat(fileContents, materialFileName);
        strcat(fileContents, "\n");
    }
    int n = 0;
    //foreach cube
    for(int x = 0; x < thisModel->x; x++){
        //detailed enough progress feedback for me
        fprintf(stdout, "%.2f%% done\r", ((float)x)/16 * 100); 
        fflush(stdout);
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct cube thisCube = thisModel->cubes[x][y][z];
                if(strcmp(thisCube.type, ignoreType) != 0 && isNotEmpty(thisCube)){
                    if(materialFileName != NULL && thisCube.m != NULL){
                        //add the usemtl line
                        size_t mtlLineSize = 9 + strlen(thisCube.m->name);
                        char* mtlLine = malloc(mtlLineSize);
                        snprintf(mtlLine, mtlLineSize, "usemtl %s\n", thisCube.m->name);
                        *outSize += mtlLineSize;
                        fileContents = realloc(fileContents, *outSize);
                        strcat(fileContents, mtlLine);
                        free(mtlLine);
                    }
                    //object definition
                    size_t objectLineSize = 11 + digits(x) + digits(y) + digits(z) + strlen(thisCube.type);
                    char* objectLine = NULL;
                    objectLine = malloc(objectLineSize);
                    snprintf(objectLine, objectLineSize, "o cube%d-%d-%d:%s\n", x, y, z, thisCube.type);
                    *outSize += objectLineSize;
                    fileContents = realloc(fileContents, *outSize);
                    strcat(fileContents, objectLine);
                    free(objectLine);
                    //foreach vertex
                    for(int i = 0; i < 8; i++){
                        struct vertex v = thisCube.vertices[i];
                        size_t size = 27 + digits((int)v.x) + digits((int)v.y) + digits((int)v.z);
                        //printf("%d %d %2f\n", size, digits(v.x), v.x);
                        char* vertexLine = NULL;
                        vertexLine = malloc(size);
                        snprintf(vertexLine, size, "v %.6f %.6f %.6f\n", v.x, v.y, v.z);
                        *outSize += size;
                        fileContents = realloc(fileContents, *outSize);
                        strcat(fileContents, vertexLine);
                        free(vertexLine);
                    }
                    //foreach face
                    for(int i = 0; i < 6; i++){
                        struct cubeFace* face = NULL;
                        face = thisCube.faces[i];
                        if(face != NULL){
                            int offset = n*8 + 1;
                            face->v1 += offset;
                            face->v2 += offset;
                            face->v3 += offset;
                            face->v4 += offset;
                            size_t size = 0;
                            char* line = NULL;
                            size = 7 + digits(face->v1) + digits(face->v2) + digits(face->v3) + digits(face->v4); 
                            line = malloc(size);
                            snprintf(line, size, "f %d %d %d %d\n", face->v1, face->v2, face->v3, face->v4); 
                            //fprintf(stderr, "%d:f %d %d %d %d-%s", i, offset + face->v1, offset + face->v2, offset + face->v3, offset + face->v4, line);
                            *outSize += size;
                            fileContents = realloc(fileContents, *outSize);
                            strcat(fileContents, line);
                            free(line);
                        }
                    }
                    n++;
                }
                
            }
        }
    }
    return fileContents;
}

void freeModel(model* m){
    for(int x = 0; x < m->x; x++){
        for(int y = 0; y < m->y; y++){
            for(int z = 0; z < m->z; z++){
                for(int i = 0; i < 6; i++){
                    if(m->cubes[x][y][z].faces[i] != NULL){
                        free(m->cubes[x][y][z].faces[i]);
                    }
                }
            }
            free(m->cubes[x][y]);
        }
        free(m->cubes[x]);
    }
    free(m->cubes);
}

struct material* getMaterials(FILE* mtlFile, int* outLen){
    struct material* result = malloc(0);
    int i = 0;
    fseek(mtlFile, 0, SEEK_END);
    long sz = ftell(mtlFile);
    fseek(mtlFile, 0, SEEK_SET); 
    char* bytes = malloc(sz + 1);
    fread(bytes, sz, 1, mtlFile);
    bytes[sz] = '\0';
    char* token = strtok(bytes, "\n");
    while(token != NULL){ //foreach line
        struct material newMaterial;
        int len = strlen(token);
        if(len > 7){
            if(token[0] == 'n' && token[4] == 't' && token[6] == ' '){
                //we are in the newmtl line
                char* name = strchr(token, ' ');
                if(name != NULL){
                    name++;
                    newMaterial.name = malloc(strlen(name) + 1);
                    strcpy(newMaterial.name, name);
                }
            }
        }
        else if(len > 2){
            if(token[0] == 'd' && token[1] == ' '){
                //we are in the d line
                char* f = strchr(token, ' ');
                if(f != NULL){
                    f++;
                    newMaterial.d = atof(f);
                    result = realloc(result, (i + 1) * sizeof(struct material));
                    result[i] = newMaterial;
                    i++;
                }
            }
        }
        token = strtok(NULL, "\n");
    }
    free(bytes);
    *outLen = i;
    return result;
}
