#include "hTable.h"

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
    struct vertex vertices[8]; //array of relative to x,y,z vertices
    struct cubeFace* faces[6]; //array of pointers so that it may be nullable
    char* type; //text representing a cube name or a broad type. Feel free to make it NULL
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
    struct material* m;
};

//A dynamic obj file object
struct object{
    float x;
    float y;
    float z;
    int vertexCount;
    struct vertex* vertices; //array of relative to x, y, z vertices
    int faceCount;
    struct objFace* faces; //doesn't need to be nullable
    struct material* m;
    char* type;
};

//model without constraints, however one that can't be culled
struct model{
    int x;
    int y;
    int z;
    struct object**** objects;
};

//Foreach object in a model*. Provides x,y,z and a not null struct object*
#define foreachObject(model) \
    for(int x = 0; x < model->x; x++) \
    for(int y = 0; y < model->y; y++) \
    for(int z = 0; z < model->z; z++) \
    if(model->objects[x][y][z] != NULL)

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

/*
Initialises a model variable and allocates necessary memory.
Returns a new reference that will need to be freed.
*/
model initModel(int x, int y, int z);

/*
Initialises a new cubeModel variable and allocates necessary memory.
Returns a new reference that will need to be freed.
*/
struct cubeModel initCubeModel(int x, int y, int z);

//Creates a new vertex
struct vertex newVertex(int x, int y, int z);

/*
Removes all outward or internal faces from a model.
All cubes that are NULL or whose type is in ignoreType are considered to be outward and faces touching those cubes are not culled.
Culled faces are free'd and set as null.
*/
int cullFaces(struct cubeModel* thisModel, bool cullChunkBorder, hashTable* specialObjects);

/*
Converts a cubeModel to a normal model, and inserts special objects if specialObjects!=NULL when types match.
Uses initModel to create a new model reference, and also creates new object references within the new model.
*/
model cubeModelToModel(struct cubeModel* m, hashTable* specialObjects);

/*
Returns a string that are valid .obj file contents. Be sure to free the contents once you are done with them.
typeArr can be NULL if you wish to not generate a textured model.
*/
char* generateModel(model* thisModel, size_t* outSize, char* materialFileName, unsigned long* offset);

//Frees all cube faces, cubes and the model itself.
void freeCubeModel(struct cubeModel* m);

//Frees everything that can be allocated in a model m
void freeModel(model* m);

//Creates and allocates necessary memory for a new cubeFace object
struct cubeFace* newCubeFace(int a, int b, int c, int d);

/*
Extracts materials from a mtl file.
Allocates necessary memory for material type strings.
Returns a new reference to a material hash table.
*/
hashTable* getMaterials(char* filename);

//Creates a clone of a material struct in memory
struct material* cloneMaterial(const struct material* input);

/*
Converts a cubeFace to the dynamic obj face.
Allocates necessary for face vertices.
*/
struct objFace deCube(struct cubeFace face);

/*
Converts a cube into an object.
Allocates memory for faces, vertices and type.
Utilizes deCube for face conversion so by proxy allocates memory for face vertices.
*/
struct object deCubeObject(struct cube* c);

/*
Returns a hash table with objects from a wavefront file. Returns NULL if fails.
*/
hashTable* readWavefront(char* filename, hashTable* materials, int side);

//Returns size of everything inside of a model
size_t getTotalModelSize(model* m);
