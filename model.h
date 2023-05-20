#include <stdlib.h>

//A block structure with it's own coordinates and a string containing it's type
struct block{
    int x;
    int y;
    int z;
    char* type;
};

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

#define mcAir "minecraft:air"
#define minY -64

//Initialises a model variable
model initModel(int x, int y, int z);

//Creates a new vertex
struct vertex newVertex(int x, int y, int z);

//Creates a new cube object based on a block object
struct cube cubeFromBlock(struct block block, const int side);

//Removes all outward or internal faces from a model
void cullFaces(model* thisModel);

//Returns a string that are valid .obj file contents
char* generateModel(model* thisModel, size_t* outSize);