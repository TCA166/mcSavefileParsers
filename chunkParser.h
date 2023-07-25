#include <stdbool.h>
#include <inttypes.h>

//16x16x16 big section of a chunk
typedef struct section{
    //we ignore the biomes because we don't need that data
    uint64_t* blockData; //raw nbt file block data
    char** blockPalette;
    size_t blockDataLen; //the length of blockData in bytes
    size_t paletteLen;
    short y;
} section;

//A block structure with it's own coordinates and a string containing it's type
typedef struct block{
    int x;
    int y;
    int z;
    char* type;
} block;

#define maxSections 24

#define mcAir "minecraft:air"
#define minY -64

/*
Extracts sections from nbtFileData of size sz into sections array and returns the number of extracted sections
blockData, blockPalette and elements of blockPalette are allocated on the heap and will need to be freed
*/
unsigned int getSections(unsigned char* nbtFileData, long sz, section* sections);

/*
Creates the block states array based on a section.
Returns NULL if it cannot be created. Be sure to free the result once you are done with it.
outLen can be NULL.
*/
unsigned int* getBlockStates(section s, int* outLen);

//Creates a block struct based on the struct coordinates, the blockstates array and the parent section
block createBlock(int x, int y, int z, unsigned int* blockStates, section parentSection);

/*
Returns an array of strings containing a complete total block palette
The only memory it allocates is the returned array
*/
char** createGlobalPalette(section* sections, int len, int* outLen, bool freeSectionPalettes);

//Frees all the allocated memory by getSections
void freeSections(section* sections, int sectionLen);