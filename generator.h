#include <stdbool.h>
#include "model.h"
#include "chunkParser.h"

//moved this here from model.c in order to properly separate that file into a proper 3d model rendering lib and a chunk nbt handling lib

/*
Creates a new cube object based on a block object.
material can be NULL
*/
cube cubeFromBlock(struct block block, const unsigned int side, struct material* material);

/*
Isolated from the main code to maybe in the future enable for a multithreaded version that can do multiple chunks.
materials can be NULL
*/
cubeModel createCubeModel(struct section* sections, int sectionLen, hashTable* materials, bool yLim, int upLim, int downLim, unsigned int side, bool matCheck, int xOff, int zOff);

//Generally speaking i could just pass a whole chunk object from regionParser here, but I like the design of having that be completely separate

/*
Generates a model object from binary nbt data.
materials and objects can be NULL. Consumes data, materials and objects entirely.
*/
model generateFromNbt(unsigned char* data, int dataSize, hashTable* materials, hashTable* objects, bool yLim, int upLim, int downLim, bool b, bool f, unsigned int side, int chunkX, int chunkZ);

//Frees the objects within a hashTable
void freeObjectsHashTable(hashTable* objects);