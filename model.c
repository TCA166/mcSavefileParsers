#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <math.h>

#include "errorDefs.h"
#include "model.h"

#define freeCubeFace(c, n) free((*c).faces[n]); (*c).faces[n] = NULL; count++;

#define faceCheck(t) t!=NULL && ((*t).m == NULL || (*t).m->d == 1) && !isPresent(t->type, specialObjects)

#define objCount 6000

#define objCoordCorrect(c, coord, dist) (c->coord * c->side) + dist

#define sq(a) ((a)*(a))

int digits(int i){
    if(i == 0){
        return 1;
    }
    double x = (double)abs(i);
    int result = (int)floor(log10(x)) + 1;
    if(i < 0){
        result++;
    }
    return result;
}

double distanceBetweenVectors(struct vertex a, struct vertex b){
    return sqrt(sq(a.x - b.x) + sq(a.y - b.y) + sq(a.z - b.z));
}

bool verticesEqual(struct vertex a, struct vertex b){
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

model initModel(int x, int y, int z){
    model newModel;
    newModel.x = x;
    newModel.y = y;
    newModel.z = z;
    newModel.objects = malloc(x * sizeof(struct object***));
    for(int i = 0; i < x; i++){
        newModel.objects[i] = malloc(y * sizeof(struct object**));
        for(int n = 0; n < y; n++){
            newModel.objects[i][n] = malloc(z * sizeof(struct object*));
        }
    }
    return newModel;
}

struct cubeModel initCubeModel(int x, int y, int z){
    struct cubeModel newModel;
    newModel.x = x;
    newModel.y = y;
    newModel.z = z;
    newModel.cubes = malloc(x * sizeof(struct cube***));
    for(int i = 0; i < x; i++){
        newModel.cubes[i] = malloc(y * sizeof(struct cube**));
        for(int n = 0; n < y; n++){
            newModel.cubes[i][n] = malloc(z * sizeof(struct cube*));
        }
    }
    return newModel;
}

size_t getTotalModelSize(model* m){
    size_t result = sizeof(int) * 3 + (sizeof(struct object*) * m->x * m->y * m->z);
    foreachObject(m){
        struct object* object = m->objects[x][y][z];
        result += sizeof(struct object) + sizeof(struct vertex) * object->vertexCount + strlen(object->type) + 1;
        if(object->m != NULL){
            result += sizeof(struct material) + strlen(object->m->name) + 1;
        }
        for(int i = 0; i < object->faceCount; i++){
            result += sizeof(struct objFace);
            result += sizeof(struct vertex) * object->faces[i].vertexCount;
            if(object->faces[i].m != NULL){
                result += sizeof(struct material) + strlen(object->faces[i].m->name) + 1;
            }
        }
    }
    return result;
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

struct objFace deCube(struct cubeFace face){
    struct objFace new;
    new.vertices = malloc(sizeof(int) * 4);
    new.vertices[0] = face.v1;
    new.vertices[1] = face.v2;
    new.vertices[2] = face.v3;
    new.vertices[3] = face.v4;
    new.vertexCount = 4;
    new.m = NULL;
    return new;
}

bool isPresent(char* string, hashTable* objects){
    if(objects == NULL){
        return 0;
    }
    struct object* ptr = getVal(objects, string);
    return ptr != NULL;
}

int cullFaces(struct cubeModel* thisModel, bool cullChunkBorder, hashTable* specialObjects){
    long count = 0;
    for(int x = 0; x < thisModel->x; x++){
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct cube* c = thisModel->cubes[x][y][z];
                if(c != NULL){
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
    return count;
}

struct object deCubeObject(struct cube* c){
    struct object result;
    float dist = c->side/2;
    result.x = objCoordCorrect(c, x, dist);
    result.y = objCoordCorrect(c, y, dist);
    result.z = objCoordCorrect(c, z, dist);
    result.faceCount = 0;
    result.faces = malloc(0);
    for(int i = 0; i < 6; i++){
        if(c->faces[i] != NULL){
            result.faces = realloc(result.faces, (result.faceCount + 1) * sizeof(struct objFace));
            result.faces[result.faceCount] = deCube(*(c->faces[i]));
            result.faceCount++;
        }
    }
    result.vertexCount = 8;
    result.vertices = malloc(8 * sizeof(struct vertex));
    memcpy(result.vertices, c->vertices, 8 * sizeof(struct vertex));
    result.m = c->m;
    result.type = malloc(strlen(c->type) + 1);
    memcpy(result.type, c->type, strlen(c->type) + 1);
    return result;
}

model cubeModelToModel(struct cubeModel* m, hashTable* specialObjects){
    model result = initModel(m->x, m->y, m->z);
    for(int x = 0; x < m->x; x++){
        for(int y = 0; y < m->y; y++){
            for(int z = 0; z < m->z; z++){
                if(m->cubes[x][y][z] != NULL){
                    char* strippedName = strchr(m->cubes[x][y][z]->type, ':') + 1;
                    if(strippedName == NULL){
                        strippedName = m->cubes[x][y][z]->type;
                    }
                    struct object* prot = (struct object*)getVal(specialObjects, strippedName);
                    result.objects[x][y][z] = malloc(sizeof(struct object));
                    if(prot != NULL){
                        memcpy(result.objects[x][y][z], prot, sizeof(struct object));
                        struct object* newObject = result.objects[x][y][z];
                        //float dist = m->cubes[x][y][z]->side/2;
                        newObject->x = m->cubes[x][y][z]->x * m->cubes[x][y][z]->side;
                        newObject->y = m->cubes[x][y][z]->y * m->cubes[x][y][z]->side; 
                        newObject->z = m->cubes[x][y][z]->z * m->cubes[x][y][z]->side;
                        //we need to copy everything so that we can free the entire hash table earlier
                        newObject->vertices = calloc(newObject->vertexCount, sizeof(struct vertex));
                        memcpy(newObject->vertices, prot->vertices, sizeof(struct vertex) * newObject->vertexCount);
                        newObject->faces = calloc(newObject->faceCount, sizeof(struct objFace));
                        memcpy(newObject->faces, prot->faces, sizeof(struct objFace) * newObject->faceCount);
                        for(int i = 0; i < newObject->faceCount; i++){
                            newObject->faces[i].vertices = calloc(prot->faces[i].vertexCount, sizeof(int));
                            memcpy(newObject->faces[i].vertices, prot->faces[i].vertices, sizeof(int) * prot->faces[i].vertexCount);
                        }
                        newObject->type = calloc(strlen(prot->type) + 1, 1);
                        strcpy(newObject->type, prot->type);

                    }
                    else{
                        *(result.objects[x][y][z]) = deCubeObject(m->cubes[x][y][z]);
                    }
                }
                else{
                    result.objects[x][y][z] = NULL;
                }
            }
        }
    }
    return result;
}

bool isNotEmpty(struct object* c){
    if(c == NULL){
        return 0;
    }
    if(c->faceCount > 0){
        return 1;
    }
    return 0;
}

//Simple procedure taken out of generateModel to prevent duplicate code
char* appendMtlLine(const char* mtlName, char* appendTo, size_t* outSize){
    size_t mtlLineSize = 9 + strlen(mtlName);
    char* mtlLine = malloc(mtlLineSize);
    snprintf(mtlLine, mtlLineSize, "usemtl %s\n", mtlName);
    *outSize += mtlLineSize - 1;
    appendTo = realloc(appendTo, *outSize);
    strcat(appendTo, mtlLine);
    free(mtlLine);
    return appendTo;
}

char* generateModel(model* thisModel, size_t* outSize, char* materialFileName, unsigned long* offset){
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
    unsigned long locOffset = 1;
    if(offset == NULL){
        offset = &locOffset;
    }
    else if(*offset < 1){
        *offset = 1; //vertex offset
    }
    //foreach object
    for(int x = 0; x < thisModel->x; x++){
        //detailed enough progress feedback for me
        fprintf(stdout, "%.2f%% done\r", ((float)x)/16 * 100); 
        fflush(stdout);
        for(int y = 0; y < thisModel->y; y++){
            for(int z = 0; z < thisModel->z; z++){
                struct object* thisObject = thisModel->objects[x][y][z];
                if(thisObject != NULL && isNotEmpty(thisObject)){
                    if(thisObject->m != NULL){
                        //add the usemtl line
                        fileContents = appendMtlLine(thisObject->m->name, fileContents, outSize);
                    }
                    //object definition
                    size_t objectLineSize = 12 + digits(x) + digits(y) + digits(z) + strlen(thisObject->type) + digits(*offset);
                    char* objectLine = NULL;
                    objectLine = malloc(objectLineSize);
                    snprintf(objectLine, objectLineSize, "o cube%d-%d-%d:%s:%ld\n", x, y, z, thisObject->type, *offset);
                    *outSize += objectLineSize - 1;
                    fileContents = realloc(fileContents, *outSize);
                    strcat(fileContents, objectLine);
                    free(objectLine);
                    //foreach vertex
                    for(int i = 0; i < thisObject->vertexCount; i++){
                        struct vertex v = thisObject->vertices[i];
                        v.x += thisObject->x;
                        v.y += thisObject->y;
                        v.z += thisObject->z;
                        size_t size = 27 + digits((int)v.x) + digits((int)v.y) + digits((int)v.z);
                        //printf("%d %d %2f\n", size, digits(v.x), v.x);
                        char* vertexLine = NULL;
                        vertexLine = malloc(size);
                        snprintf(vertexLine, size, "v %.6f %.6f %.6f\n", v.x, v.y , v.z);
                        *outSize += size  - 1;
                        fileContents = realloc(fileContents, *outSize);
                        strcat(fileContents, vertexLine);
                        free(vertexLine);
                    }
                    //foreach face
                    for(int i = 0; i < thisObject->faceCount; i++){
                        struct objFace face = thisObject->faces[i];
                        if(face.m != NULL){
                            fileContents = appendMtlLine(face.m->name, fileContents, outSize);
                        }
                        size_t size = 4;
                        for(int m = 0; m < face.vertexCount; m++){
                            face.vertices[m] += *offset;
                            size += digits(face.vertices[m]) + 1;
                        }
                        char* line = malloc(size);
                        line[0] = '\0';
                        strcat(line, "f ");
                        int lineOff = 2;
                        for(int m = 0; m < face.vertexCount; m++){
                            size_t len = digits(face.vertices[m]) + 1;
                            snprintf(line + lineOff, len + 1, "%d ", face.vertices[m]);
                            lineOff += len;
                        }
                        strcat(line, "\n");
                        *outSize += size - 1;
                        fileContents = realloc(fileContents, *outSize);
                        strcat(fileContents, line);
                        free(line);
                    }
                    *offset += thisObject->vertexCount;
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
                if(m->objects[x][y][z] != NULL){
                    for(int i = 0; i < m->objects[x][y][z]->faceCount; i++){
                        free(m->objects[x][y][z]->faces[i].vertices);
                    }
                    free(m->objects[x][y][z]->faces);
                    free(m->objects[x][y][z]->vertices);
                    free(m->objects[x][y][z]->type);
                }
                free(m->objects[x][y][z]);
            }
            free(m->objects[x][y]);
        }
        free(m->objects[x]);
    }
    free(m->objects);
}

void freeCubeModel(struct cubeModel* m){
    for(int x = 0; x < m->x; x++){
        for(int y = 0; y < m->y; y++){
            for(int z = 0; z < m->z; z++){
                if(m->cubes[x][y][z] != NULL){
                    for(int i = 0; i < 6; i++){
                        free(m->cubes[x][y][z]->faces[i]);
                    }
                }
                free(m->cubes[x][y][z]);
            }
            free(m->cubes[x][y]);
        }
        free(m->cubes[x]);
    }
    free(m->cubes);
}

hashTable* getMaterials(char* filename){
    FILE* mtlFile = fopen(filename, "r");
    if(mtlFile == NULL){
        fileError(filename, "opened");
    }
    if(fseek(mtlFile, 0, SEEK_END) != 0){
        fileError(filename, "seek");
    }
    long sz = ftell(mtlFile);
    if(fseek(mtlFile, 0, SEEK_SET) != 0){
        fileError(filename, "seek");
    } 
    char* bytes = malloc(sz + 1);
    if(fread(bytes, sz, 1, mtlFile) != 1){
        fileError(filename, "read");
    }
    bytes[sz] = '\0';
    hashTable* result = initHashTable(objCount);
    char* token = strtok(bytes, "\n");
    struct material newMaterial;
    while(token != NULL){ //foreach line
        int len = strlen(token);
        if(len > 7){
            if(token[0] == 'n' && token[4] == 't' && token[6] == ' '){
                //we are in the new mtl line
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
                    struct material* ptr = malloc(sizeof(struct material));
                    *ptr = newMaterial;
                    insertHashItem(result, newMaterial.name, ptr);
                }
            }
        }
        token = strtok(NULL, "\n");
    }
    free(bytes);
    return result;
}

hashTable* readWavefront(char* filename, hashTable* materials, int side){
    FILE* fp = fopen(filename, "r");
    if(fp == NULL){
        return NULL;
    }
    if(fseek(fp, 0, SEEK_END) != 0){
        fileError(filename, "seek");
    }
    long sz = ftell(fp);
    if(fseek(fp, 0, SEEK_SET) != 0){
        fileError(filename, "seek");
    }
    char* bytes = malloc(sz + 1);
    if(fread(bytes, sz, 1, fp) != 1){
        fileError(filename, "read");
    }
    bytes[sz] = '\0'; //done to fix an invalid read
    if(fclose(fp) == EOF){
        fileError(filename, "closed");
    }
    hashTable* result = initHashTable(objCount); 
    char* token = strtok(bytes, "\n");
    struct object newObject;
    newObject.type = NULL;
    newObject.vertices = NULL;
    newObject.faces = NULL;
    newObject.x = -1;
    newObject.y = -1;
    newObject.z = -1;
    struct material* nextM = NULL;
    while(token != NULL){
        switch(token[0]){
            case 'u':;
                if(materials != NULL){
                    char* mtlName = strchr(token, ' ');
                    mtlName++;
                    nextM = (struct material*)getVal(materials, mtlName);
                }
                break;
            case 'o':;
                //something is up here with key
                if(newObject.type != NULL){
                    if(newObject.vertexCount > 0 && newObject.faceCount > 0){
                        struct object* ptr = malloc(sizeof(struct object));
                        *ptr = newObject;
                        insertHashItem(result, newObject.type, ptr);
                    }
                    //free(newObject.type);
                }
                //this should 'reset' the newObject
                newObject.faceCount = 0;
                //free(newObject.faces);
                newObject.faces = malloc(0);
                newObject.vertexCount = 0;
                //free(newObject.vertices);
                newObject.vertices = malloc(0);
                if(nextM != NULL){
                    newObject.m = nextM;
                    nextM = NULL;
                }
                else{
                    newObject.m = NULL;
                }
                char* name = strchr(token, ' ');
                name++;
                //these two lines cause a very weird memory error
                newObject.type = malloc(strlen(name) + 1);
                strcpy(newObject.type, name);
                break;
            case 'v':;
                float x;
                float y;
                float z;
                sscanf(token, "v %f %f %f", &x, &y, &z);
                if(newObject.vertices == NULL){
                    newObject.vertices = malloc(sizeof(struct vertex));
                }
                else{
                    newObject.vertices = realloc(newObject.vertices, (newObject.vertexCount + 1) * sizeof(struct vertex));
                }
                newObject.vertices[newObject.vertexCount] = newVertex(x * side, y * side, z * side);
                newObject.vertexCount++;
                break;
            case 'f':;
                int* vertices = malloc(0);
                char* num = calloc(1, 1);
                num[0] = '\0';
                int n = 0;
                int f = 0;
                for(int i = 2; i < strlen(token); i++){
                    if(token[i] == ' '){
                        if(n > 0){
                            //parse the num string, append the result to vertices and reset num
                            vertices = realloc(vertices, (f + 1) * sizeof(int));
                            vertices[f] = atoi(num);
                            if(vertices[f] > newObject.vertexCount){
                                vertexWarning(newObject.type);
                            }
                            f++;
                            num = realloc(num, 1);
                            num[0] = '\0';
                        }
                        n = 0;
                    }
                    else{
                        //append to the WIP number string that will be parsed by atoi
                        num = realloc(num, n + 2);
                        num[n] = token[i];
                        num[n + 1] = '\0';
                        n++;
                    }
                }
                if(strlen(num) > 0){
                    vertices = realloc(vertices, (f + 1) * sizeof(int));
                    vertices[f] = atoi(num);
                    f++;
                }
                free(num);
                struct objFace newFace;
                newFace.vertexCount = f;
                newFace.vertices = vertices;
                if(nextM != NULL){
                    newFace.m = nextM;
                    nextM = NULL;
                }
                else{
                    newFace.m = NULL;
                }
                if(newObject.faces == NULL){
                    newObject.faces = malloc(sizeof(struct objFace));
                }
                else{
                    newObject.faces = realloc(newObject.faces, (newObject.faceCount + 1) * sizeof(struct objFace));
                }
                newObject.faces[newObject.faceCount] = newFace;
                newObject.faceCount++;
                break;
        }
        token = strtok(NULL, "\n");
    }
    free(bytes);
    if(newObject.vertexCount > 0 && newObject.faceCount > 0){
        struct object* ptr = malloc(sizeof(struct object));
        *ptr = newObject;
        insertHashItem(result, newObject.type, ptr);
    }
    return result;
}
