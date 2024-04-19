#include "hTable.h"
#include <stdbool.h>

//mtl material
typedef struct material{
    char* name;
    //here i could add more fields... but field d is the only one we need
    float d;
} material;

//obj file vertex
typedef struct vertex{
    float x;
    float y;
    float z;
} vertex;

//A face with only 4 vertices. V variables point to vertices in the associated cube object
typedef struct cubeFace{
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
    unsigned int v4;
} cubeFace;

//obj file cube with 6 cubeFaces and 8 defining vertices
typedef struct cube{
    float x;
    float y;
    float z;
    float side; //Cannot be lower than 0 for very obvious reasons
    vertex vertices[8]; //array of relative to x,y,z vertices
    cubeFace* faces[6]; //array of pointers so that it may be nullable
    char* type; //text representing a cube name or a broad type. Feel free to make it NULL
    material* m;
} cube;

/*
A strict 3D model struct that is made up of cubes in a strict 3D arrangement.
Can be transformed into a normal loose model.
Ideally if you plan on generating a model of something that can be represented by cubes you first generate this.
*/
typedef struct cubeModel{
    unsigned int x; //Max x index
    unsigned int y; //Max y index
    unsigned int z; //Max z index
    struct cube**** cubes; //nullable 3d array of cubes
} cubeModel;

//A dynamic array of vertices - an object face
typedef struct objFace{
    size_t vertexCount;
    int* vertices;
    material* m;
} objFace;

//A dynamic obj file object
typedef struct object{
    float x;
    float y;
    float z;
    size_t vertexCount;
    vertex* vertices; //array of relative to x, y, z vertices
    size_t faceCount;
    objFace* faces; //doesn't need to be nullable
    material* m;
    char* type;
} object;

//The idea behind the materialArr is that it's way more memory efficient to store materials once rather than possibly hundreds of times

/*
A loose array of even looser objects with an associated array of materials that objects can point to.
*/
typedef struct model{
    object** objects; //Nullable array of objects
    size_t objectCount; //Size of objects
    material** materialArr; //Optional array of materials, that objects can point to
    size_t materialCount; //Size of materialArr
} model;

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

//Type for holding vertex offset (unsigned long)
typedef unsigned long offset_t;

/*
Initialises a model variable and allocates necessary memory.
Returns a new reference that will need to be freed.
*/
model initModel(int objectCount);

/*
Initialises a new cubeModel variable with dimensions x*y*z cubes and allocates necessary memory.
Returns a new reference that will need to be freed.
*/
cubeModel initCubeModel(int x, int y, int z);

//Creates a new vertex
vertex newVertex(int x, int y, int z);

/*
Removes all outward or internal faces from a model.
All cubes that are NULL or whose type is in ignoreType are considered to be outward and faces touching those cubes are not culled.
Culled faces are free'd and set as null.
*/
unsigned int cullFaces(cubeModel* thisModel, bool cullChunkBorder, hashTable* specialObjects);

/*
Converts a cubeModel to a normal model, and inserts special objects if specialObjects!=NULL when types match.
Uses initModel to create a new model reference, and also creates new object references within the new model.
*/
model cubeModelToModel(const cubeModel* m, hashTable* specialObjects);

/*
Returns a string that are valid .obj file contents. Be sure to free the contents once you are done with them.
outSize, materialFileName and offset can be NULL, if you wish to not get those values back.
If materialFileName is NULL the mttlib line will not be added, but usemtl will be generated normally.
*/
char* generateModel(const model* thisModel, size_t* outSize, const char* materialFileName, offset_t* offset);

//Frees all cube faces, cubes and the model itself. Doesn't free the material.
void freeCubeModel(cubeModel* m);

//Frees everything that can be allocated in a model m. This includes the materials
void freeModel(model* m);

//Creates and allocates necessary memory for a new cubeFace object
cubeFace* newCubeFace(int a, int b, int c, int d);

/*
Extracts materials from a mtl file.
Allocates necessary memory for material type strings.
Returns a new reference to a material hash table.
*/
hashTable* getMaterials(char* filename);

//Creates a clone of a material struct in memory
material* cloneMaterial(const material* input);

/*
Converts a cubeFace to the dynamic obj face.
Allocates necessary for face vertices.
*/
objFace deCube(cubeFace face);

/*
Converts a cube into an object.
Allocates memory for faces, vertices and type.
Utilizes deCube for face conversion so by proxy allocates memory for face vertices.
*/
object deCubeObject(cube* c);

/*
Returns a hash table with objects from a wavefront file. Returns NULL if fails.
*/
hashTable* readWavefront(char* filename, hashTable* materials, unsigned int side);

//Returns the distance between two vectors
double distanceBetweenVectors(vertex a, vertex b);

//Returns true if two vertices are equal
bool verticesEqual(vertex a, vertex b);

//Returns the number of vertices in the model. Should be equivalent to the offset value if the model were to be generated -1
offset_t getTotalVertexCount(model m);

//Returns a generic cube object
cube createGenericCube(unsigned int side);

/*
Converts a model to a single well optimized object.
Removes overlapping vertices.
type can be null, but shouldn't be free'd as it's assigned to the result.
*/
object modelToObject(const model* m, const char* type);
