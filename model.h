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
    struct material* m;
};

//A face with only 4 vertices. V variables point to vertices in the associated cube object
struct cubeFace{
    int v1;
    int v2;
    int v3;
    int v4;
};

//A dynamic array of vertices - an object face
struct objFace{
    int vertexCount;
    int* vertices;
};

//A dynamic obj file object
struct object{
    float x;
    float y;
    float z;
    int vertexCount;
    struct vertex* vertices;
    int faceCount;
    struct objFace* faces; //doesn't need to be nullable
    struct material* m;
};

//model without constraints, however one that can't be culled
struct model{
    int x;
    int y;
    int z;
    struct object**** objects;
};

//model with constraints, however can be culled
struct cubeModel{
    int x;
    int y;
    int z;
    struct cube**** cubes; //nullable 3d array of cubes
};

//mtl material
struct material{
    char* name;
    //here i could add more fields... but field d is the only one we need
    float d;
};

//3d array of blocks
typedef struct model model;

//Initialises a model variable
model initModel(int x, int y, int z);

//Initialises a new cubeModel variable
struct cubeModel initCubeModel(int x, int y, int z);

//Creates a new vertex
struct vertex newVertex(int x, int y, int z);

//Removes all outward or internal faces from a model
void cullFaces(struct cubeModel* thisModel, char cullChunkBorder);

//Converts a cubeModel to a normal model
model cubeModelToModel(struct cubeModel* m);

//Returns a string that are valid .obj file contents. Be sure to free the contents once you are done with them.
//typeArr can be NULL if you wish to not generate a textured model
char* generateModel(model* thisModel, size_t* outSize, char* materialFileName);

void freeCubeModel(struct cubeModel* m);

//Frees everything that can be allocated in a model m
void freeModel(model* m);

//Creates and allocated necessary memory for a new cubeFace object
struct cubeFace* newCubeFace(int a, int b, int c, int d);

//Extracts materials from a mtl file
struct material* getMaterials(FILE* mtlFile, int* outLen);

//Converts a cubeface to the dynamic obj face
struct objFace deCube(struct cubeFace face);

//Converts a cube into an object
struct object deCubeObject(struct cube* c);

//Converts a cubeModel to a object model
model cubeModelToModel(struct cubeModel* m);

//Returns an array of objects from a .obj file with the given filename. Outputs into outlen the amount of returned objects, into objectNames an array of names of the extracted objects.
//Allocates memory for objectNames (if it isn't NULL), for the objects themselves and ofcourse for readWavefront. Returns NULL if it can't open filename
struct object* readWavefront(char* filename, int* outLen, char** objectNames, struct material* materials, int materialLen);