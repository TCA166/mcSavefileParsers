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

#define statesFormula(x, y, z) (y * 16 + z) * 16 + x

//Extracts sections from nbtFileData of size sz into sections and returns the number of extracted sections
int getSections(unsigned char* nbtFileData, long sz, struct section* sections);

/*Creates the block states array based on a section.
Returns NULL if it cannot be created. Be sure to free the result once you are done with it.
*/
unsigned int* getBlockStates(struct section s);

//Creates a block struct based on the coordinates, side, the blockstates array and the parent section
struct block createBlock(int x, int y, int z, unsigned int* blockStates, int side, struct section parentSection);