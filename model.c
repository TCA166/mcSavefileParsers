#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "model.h"

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

struct cubeFace* newCubeFace(int a, int b, int c, int d){
    struct cubeFace face;
    face.v1 = a;
    face.v2 = b;
    face.v3 = c;
    face.v4 = d;
    struct cubeFace* ptr = &face;
    return ptr;
}

struct cube cubeFromBlock(struct block block, const int side){
    struct cube newCube;
    float dist = side/2;
    newCube.side = side;
    newCube.x = block.x + dist;
    newCube.y = block.y + dist;
    newCube.z = block.z + dist;
    //binary 8 to 0
    newCube.vertices[0] = newVertex(newCube.x + dist, newCube.y + dist, newCube.z + dist);
    newCube.vertices[1] = newVertex(newCube.x + dist, newCube.y + dist, newCube.z - dist);
    newCube.vertices[2] = newVertex(newCube.x + dist, newCube.y - dist, newCube.z + dist);
    newCube.vertices[3] = newVertex(newCube.x + dist, newCube.y - dist, newCube.z - dist);
    newCube.vertices[4] = newVertex(newCube.x - dist, newCube.y + dist, newCube.z + dist);
    newCube.vertices[5] = newVertex(newCube.x - dist, newCube.y + dist, newCube.z - dist);
    newCube.vertices[6] = newVertex(newCube.x - dist, newCube.y - dist, newCube.z + dist);
    newCube.vertices[7] = newVertex(newCube.x - dist, newCube.y - dist, newCube.z - dist);
    
    newCube.faces[0] = newCubeFace(0, 1, 2, 3); //right +x
    newCube.faces[1] = newCubeFace(0, 1, 4, 5); //up +y
    newCube.faces[2] = newCubeFace(0, 2, 4, 6); //forward +z
    newCube.faces[3] = newCubeFace(7, 3, 1, 5); //back -z
    newCube.faces[4] = newCubeFace(7, 5, 4, 6); //left -x
    newCube.faces[5] = newCubeFace(7, 6, 2, 3); //down -y

    newCube.type = block.type;
    return newCube;
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

void cullFaces(model* thisModel){
    for(int x = 0; x < thisModel->x; x++){
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct cube c = thisModel->cubes[x][y][z];
                //x
                if( x + 1 >= thisModel->x){
                    c.faces[0] = NULL;
                }
                else if(strcmp(thisModel->cubes[x + 1][y][z].type, mcAir)){
                    c.faces[0] = NULL;
                }
                if( x - 1 < 0){
                    c.faces[4] = NULL;
                }
                else if(strcmp(thisModel->cubes[x - 1][y][z].type, mcAir)){
                    c.faces[4] = NULL;
                }
                //y
                if( y + 1 >= thisModel->y){
                    c.faces[1] = NULL;
                }
                else if(strcmp(thisModel->cubes[x][y + 1][z].type, mcAir)){
                    c.faces[1] = NULL;
                }
                if( y - 1 < 0){
                    c.faces[5] = NULL;
                }
                else if(strcmp(thisModel->cubes[x][y - 1][z].type, mcAir)){
                    c.faces[5] = NULL;
                }
                //z
                if( z + 1 >= thisModel->z){
                    c.faces[2] = NULL;
                }
                else if(strcmp(thisModel->cubes[x][y][z + 1].type, mcAir)){
                    c.faces[2] = NULL;
                }
                if( z - 1 < 0){
                    c.faces[3] = NULL;
                }
                else if(strcmp(thisModel->cubes[x][y][z - 1].type, mcAir)){
                    c.faces[3] = NULL;
                }
                thisModel->cubes[x][y][z] = c;
            }
        }
    }
}

char* generateModel(model* thisModel, size_t* outSize){
    char* fileContents = malloc(*outSize + 1);
    fileContents[0] = '\0';
    //foreach cube
    for(int x = 0; x < thisModel->x; x++){
        fprintf(stdout, "%.2f%% done\r", ((float)x)/16 * 100);
        fflush(stdout);
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct cube thisCube = thisModel->cubes[x][y][z];
                if(thisCube.type != mcAir){
                    size_t objectLineSize = 6 + digits(x) + digits(y) + digits(z);
                    char* objectLine = malloc(objectLineSize);
                    objectLine[0] = '\0';
                    snprintf(objectLine, objectLineSize, "o %d-%d-%d\n", x, y, z);
                    *outSize += objectLineSize;
                    fileContents = realloc(fileContents, *outSize);
                    strcat(fileContents, objectLine);
                    free(objectLine);
                    //foreach vertex
                    for(int i = 0; i < 8; i++){
                        struct vertex v = thisCube.vertices[i];
                        size_t size = 15 + digits((int)v.x) + digits((int)v.y) + digits((int)v.z);
                        //printf("%d %d %2f\n", size, digits(v.x), v.x);
                        char* line = malloc(size);
                        line[0] = '\0';
                        snprintf(line, size, "v %.2f %.2f %.2f\n", v.x, v.y, v.z);
                        *outSize += size;
                        fileContents = realloc(fileContents, *outSize);
                        strcat(fileContents, line);
                        free(line);
                    }
                    //foreach face
                    for(int i = 0; i < 6; i++){
                        struct cubeFace* face = thisCube.faces[i];
                        if(face != NULL){
                            //Conditional jump or move depends on uninitialised value here in log10
                            size_t size = 7 + digits(face->v1) + digits(face->v2) + digits(face->v3) + digits(face->v4); 
                            char* line = malloc(size);
                            line[0] = '\0';
                            //Conditional jump or move depends on uninitialised value here
                            snprintf(line, size, "f %d %d %d %d\n", face->v1, face->v2, face->v3, face->v4); 
                            *outSize += size;
                            fileContents = realloc(fileContents, *outSize);
                            strcat(fileContents, line);
                            free(line);
                        }
                    }
                }
                
            }
        }
    }
    return fileContents;
}