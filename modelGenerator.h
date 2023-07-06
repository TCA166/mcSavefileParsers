#include "model.h"
#include "chunkParser.h"

//moved this here from model.c in order to properly separate that file into a proper 3d model rendering lib and a chunk nbt handling lib

//Creates a new cube object based on a block object
struct cube cubeFromBlock(struct block block, const int side, struct material* material);

//Isolated from the main code to maybe in the future enable for a multithreaded version that can do multiple chunks
struct cubeModel createCubeModel(struct section* sections, int sectionLen, hashTable* materials, bool yLim, int upLim, int downLim, int side, bool matCheck);

//Generates a model object from binary nbt data
model generateFromNbt(unsigned char* data, long dataSize, hashTable* materials, hashTable* objects, bool yLim, int upLim, int downLim, bool b, bool f, int side);