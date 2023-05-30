#include <stdlib.h>

//16x16x16 big section of a chunk
struct section{
    short y;

    //we ignore the biomes because we don't need that data

    unsigned long* blockData; //raw nbt file block data
    int blockDataLen;
    char** blockPalette;
    int paletteLen;
};

//A block structure with it's own coordinates and a string containing it's type
struct block{
    int x;
    int y;
    int z;
    char* type;
};

#define maxSections 24

#define createMask(startBit, X) (((long)1 << X) - 1) << startBit

#define statesFormula(x, y, z) (y*16*16) + (z*16) + x

#define mcAir "minecraft:air"
#define minY -64

/*
Extracts sections from nbtFileData of size sz into sections array and returns the number of extracted sections
blockData, blockPalette and elements of blockPalette are allocated on the heap and will need to be freed
*/
int getSections(unsigned char* nbtFileData, long sz, struct section* sections);

/*
Creates the block states array based on a section.
Returns NULL if it cannot be created. Be sure to free the result once you are done with it.
outLen can be NULL.
*/
unsigned int* getBlockStates(struct section s, int* outLen);

//Creates a block struct based on the struct coordinates, the blockstates array and the parent section
struct block createBlock(int x, int y, int z, unsigned int* blockStates, struct section parentSection);

//Returns an array of strings containing a complete total block palette
//The only memory it allocates is the returned array
char** createGlobalPalette(struct section* sections, int len, int* outLen, char freeSectionPalletes);

//Frees all the allocated memory by getSections
void freeSections(struct section* sections, int sectionLen);