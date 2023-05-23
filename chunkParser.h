#include <stdlib.h>

//16x16x16 big section of a chunk
struct section{
    short y;

    //we ignore the biomes because we don't need that data

    unsigned long* blockData;
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

//Extracts sections from nbtFileData of size sz into sections and returns the number of extracted sections
int getSections(unsigned char* nbtFileData, long sz, struct section* sections);