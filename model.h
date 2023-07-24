#include "hTable.h"
#include <stdbool.h>

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
    float side; //Cannot be lower than 0 for very obvious reasons
    struct vertex vertices[8]; //array of relative to x,y,z vertices
    struct cubeFace* faces[6]; //array of pointers so that it may be nullable
    char* type; //text representing a cube name or a broad type. Feel free to make it NULL
    struct material* m;
};

//A face with only 4 vertices. V variables point to vertices in the associated cube object
struct cubeFace{
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
    unsigned int v4;
};

//A dynamic array of vertices - an object face
struct objFace{
    unsigned int vertexCount;
    int* vertices;
    struct material* m;
};

//A dynamic obj file object
struct object{
    float x;
    float y;
    float z;
    unsigned int vertexCount;
    struct vertex* vertices; //array of relative to x, y, z vertices
    unsigned int faceCount;
    struct objFace* faces; //doesn't need to be nullable
    struct material* m;
    char* type;
};

//The idea behind the materialArr is that it's way more memory efficient to store materials once rather than possibly hundreds of times

/*
A loose array of even looser objects with an associated array of materials that objects can point to.
*/
struct model{
    struct object** objects; //Nullable array of objects
    unsigned int objectCount; //Size of objects
    struct material** materialArr; //Optional array of materials, that objects can point to
    unsigned int materialCount; //Size of materialArr
};

#define dimensionalFor(xLim, yLim, zLim) \
    for(int x = 0; x < xLim; x++) \
    for(int y = 0; y < yLim; y++) \
    for(int z = 0; z < zLim; z++)

//Foreach object in a model*. Provides index variable o, and assures that the object will not be null
#define foreachObject(model) \
    for(int o = 0; o < model->objectCount; o++) \
    if(model->objects[o] != NULL)

//Foreach cube in a cubeModel*. Provides x, y and z, and assures that the cube will not be null
#define foreachCube(cubeModel) \
    dimensionalFor(cubeModel->x, cubeModel->y, cubeModel->z) \
    if(cubeModel->cubes[x][y][z] != NULL)

/*
A strict 3D model struct that is made up of cubes in a strict 3D arrangement.
Can be transformed into a normal loose model.
Ideally if you plan on generating a model of something that can be represented by cubes you first generate this.
*/
struct cubeModel{
    unsigned int x; //Max x index
    unsigned int y; //Max y index
    unsigned int z; //Max z index
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
model initModel(int objectCount);

/*
Initialises a new cubeModel variable with dimensions x*y*z cubes and allocates necessary memory.
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
unsigned int cullFaces(struct cubeModel* thisModel, bool cullChunkBorder, hashTable* specialObjects);

/*
Converts a cubeModel to a normal model, and inserts special objects if specialObjects!=NULL when types match.
Uses initModel to create a new model reference, and also creates new object references within the new model.
*/
model cubeModelToModel(const struct cubeModel* m, hashTable* specialObjects);

/*
Returns a string that are valid .obj file contents. Be sure to free the contents once you are done with them.
typeArr can be NULL if you wish to not generate a textured model.
*/
char* generateModel(const model* thisModel, size_t* outSize, char* materialFileName, unsigned long* offset);

//Frees all cube faces, cubes and the model itself. Doesn't free the material.
void freeCubeModel(struct cubeModel* m);

//Frees everything that can be allocated in a model m. This includes the materials
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
hashTable* readWavefront(char* filename, hashTable* materials, unsigned int side);

//Returns the distance between two vectors
double distanceBetweenVectors(struct vertex a, struct vertex b);

//Returns true if two vertices are equal
bool verticesEqual(struct vertex a, struct vertex b);

//Returns the number of vertices in the model. Should be equivalent to the offset value if the model were to be generated -1
unsigned long getTotalVertexCount(model m);

//Returns a generic cube object
struct cube createGenericCube(unsigned int side);

/*
Converts a model to a single well optimized object.
Removes overlapping vertices.
type can be null, but shouldn't be free'd as it's assigned to the result.
*/
struct object modelToObject(const model* m, const char* type);
