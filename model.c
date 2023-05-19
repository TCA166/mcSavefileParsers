#include <stdlib.h>

#include "model.h"

model initModel(int x, int y, int z){
    model newModel;
    newModel.x = x;
    newModel.y = y;
    newModel.z = z;
    newModel.cubes = malloc(x * sizeof(struct cube**));
    for(int i = 0; i <= x; i++){
        newModel.cubes[i] = malloc(y * sizeof(struct cube*));
        for(int n = 0; n <= y; n++){
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

struct cubeFace* newCubeFace(struct vertex a, struct vertex b, struct vertex c, struct vertex d){
    struct cubeFace face;
    face.vertices[0] = a;
    face.vertices[1] = b;
    face.vertices[2] = c;
    face.vertices[3] = d;
    struct cubeFace* ptr = &face;
    return ptr;
}

struct cube cubeFromBlock(struct block block, const int side){
    struct cube newCube;
    newCube.side = side;
    newCube.x = block.x;
    newCube.y = block.y;
    newCube.z = block.z;
    int dist = side/2;
    //binary 8 to 0
    newCube.vertices[0] = newVertex(newCube.x + dist, newCube.y + dist, newCube.z + dist);
    newCube.vertices[1] = newVertex(newCube.x + dist, newCube.y + dist, newCube.z - dist);
    newCube.vertices[2] = newVertex(newCube.x + dist, newCube.y - dist, newCube.z + dist);
    newCube.vertices[3] = newVertex(newCube.x + dist, newCube.y - dist, newCube.z - dist);
    newCube.vertices[4] = newVertex(newCube.x - dist, newCube.y + dist, newCube.z + dist);
    newCube.vertices[5] = newVertex(newCube.x - dist, newCube.y + dist, newCube.z - dist);
    newCube.vertices[6] = newVertex(newCube.x - dist, newCube.y - dist, newCube.z + dist);
    newCube.vertices[7] = newVertex(newCube.x - dist, newCube.y - dist, newCube.z - dist);
    
    newCube.faces[0] = newCubeFace(newCube.vertices[0], newCube.vertices[1], newCube.vertices[2], newCube.vertices[3]); //right +x
    newCube.faces[1] = newCubeFace(newCube.vertices[0], newCube.vertices[1], newCube.vertices[4], newCube.vertices[5]); //up +y
    newCube.faces[2] = newCubeFace(newCube.vertices[0], newCube.vertices[2], newCube.vertices[4], newCube.vertices[6]); //forward +z
    newCube.faces[3] = newCubeFace(newCube.vertices[7], newCube.vertices[3], newCube.vertices[1], newCube.vertices[5]); //back -z
    newCube.faces[4] = newCubeFace(newCube.vertices[7], newCube.vertices[5], newCube.vertices[4], newCube.vertices[6]); //left -x
    newCube.faces[5] = newCubeFace(newCube.vertices[7], newCube.vertices[6], newCube.vertices[2], newCube.vertices[3]); //down -y

    newCube.type = block.type;
    return newCube;
}

struct face deCube(struct cubeFace face){
    struct face new;
    new.vertices = malloc(sizeof(struct vertex) * 4);
    new.vertices[0] = face.vertices[0];
    new.vertices[1] = face.vertices[1];
    new.vertices[2] = face.vertices[2];
    new.vertices[3] = face.vertices[3];
    return new;
}

void cullFaces(model* thisModel){
    for(int x = 0; x < thisModel->x; x++){
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct cube c = thisModel->cubes[x][y][z];
                //x
                if( x + 1 > thisModel->x){
                    c.faces[0] = NULL;
                }
                else if(thisModel->cubes[x + 1][y][z].type == mcAir){
                    c.faces[0] = NULL;
                }
                if( x - 1 < 0){
                    c.faces[4] = NULL;
                }
                else if(thisModel->cubes[x - 1][y][z].type == mcAir){
                    c.faces[4] = NULL;
                }
                //y
                if( y + 1 > thisModel->y){
                    c.faces[1] = NULL;
                }
                else if(thisModel->cubes[x][y + 1][z].type == mcAir){
                    c.faces[1] = NULL;
                }
                if( y - 1 < 0){
                    c.faces[5] = NULL;
                }
                else if(thisModel->cubes[x][y - 1][z].type == mcAir){
                    c.faces[5] = NULL;
                }
                //z
                if( z + 1 > thisModel->z){
                    c.faces[2] = NULL;
                }
                else if(thisModel->cubes[x][y][z + 1].type == mcAir){
                    c.faces[2] = NULL;
                }
                if( z - 1 < 0){
                    c.faces[3] = NULL;
                }
                else if(thisModel->cubes[x][y][z - 1].type == mcAir){
                    c.faces[3] = NULL;
                }
                thisModel->cubes[x][y][z] = c;
            }
        }
    }
}

char* generateModel(model* thisModel){

}