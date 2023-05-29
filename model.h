#include <stdlib.h>

//obj file vertex
struct vertex{
    float x;
    float y;
    float z;
};

//obj file cube with 6 cubeFaces and 8 defining vertices
struct cube{
    float x;
    float y;
    float z;
    float side;
    struct vertex vertices[8];
    struct cubeFace* faces[6]; //array of pointers so that it may be nullable
    char* type;
};

//A face with only 4 vertices. V variables point to vertices in the associated cube object
struct cubeFace{
    int v1;
    int v2;
    int v3;
    int v4;
};

//obj file face
struct face{
    struct vertex* vertices;
};

struct model{
    int x;
    int y;
    int z;
    struct cube*** cubes;
};

//3d array of blocks
typedef struct model model;

#define freeCubeFace(c, n) free(c.faces[n]); c.faces[n] = NULL;

//Initialises a model variable
model initModel(int x, int y, int z);

//Creates a new vertex
struct vertex newVertex(int x, int y, int z);

//Removes all outward or internal faces from a model
void cullFaces(model* thisModel, char cullChunkBorder, char* ignoreType);

//Returns a string that are valid .obj file contents. Be sure to free the contents once you are done with them.
//typeArr can be NULL if you wish to not generate a textured model
char* generateModel(model* thisModel, size_t* outSize, char* ignoreType, char** typeArr, int materialLen, char* materialFileName);

//Frees everything that can be allocated in a model m
void freeModel(model* m);

//Creates and allocated necessary memory for a new cubeFace object
struct cubeFace* newCubeFace(int a, int b, int c, int d);