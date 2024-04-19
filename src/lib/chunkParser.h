/*!
 @file chunkParser.h
 @brief Contains functions for parsing chunk data
 @details This file contains functions for parsing chunk data
*/

/*!
 @defgroup chunkParser Chunk Parser
 @brief Chunk data parsing functions
*/

#include <stdbool.h>
#include <inttypes.h>

/*!
 @struct section
 @brief 16x16x16 section of a chunk
 @ingroup chunkParser
*/
typedef struct section{
    //we ignore the biomes because we don't need that data
    uint64_t* blockData; //raw nbt file block data
    char** blockPalette;
    size_t blockDataLen; //the length of blockData in bytes
    size_t paletteLen;
    short y;
} section;

/*!
 @struct block
 @brief A block structure with it's own coordinates and a string containing it's type
 @ingroup chunkParser
*/
typedef struct block{
    int x;
    int y;
    int z;
    char* type;
} block;

/*!
 @def maxSections
 @brief The maximum number of sections in a chunk
 @details This is the maximum number of sections in a chunk, which is 24 as of 1.17
 @ingroup chunkParser
*/
#define maxSections 24

/*!
 @def mcAir
 @brief The minecraft air block identifier
 @ingroup chunkParser
*/
#define mcAir "minecraft:air"
/*!
 @def minY
 @brief The minimum y coordinate
 @ingroup chunkParser
*/
#define minY -maxSections*4

/*!
 @brief Extracts sections from raw nbtFileData of size into sections array
 @details blockData, blockPalette and elements of blockPalette are allocated on the heap and will need to be freed
 @param nbtFileData The raw nbt file data
 @param sz The size of the nbt file data
 @param sections The array of sections to be filled
 @return The number of sections extracted
 @ingroup chunkParser
*/
unsigned int getSections(unsigned char* nbtFileData, long sz, section* sections);

/*!
 @brief Creates the block states array based on a section.
 @details Returns NULL if it cannot be created. Be sure to free the result once you are done with it.
 @param s The section to create the block states from
 @param outLen The length of the block states array
 @return The block states array
 @ingroup chunkParser
*/
unsigned int* getBlockStates(section s, int* outLen);

/*!
 @brief Creates a block struct based on the struct coordinates, the blockstates array and the parent section
 @param x The x coordinate of the block
 @param y The y coordinate of the block
 @param z The z coordinate of the block
 @param blockStates The block states array
 @param parentSection The parent section of the block
 @return The block struct
 @ingroup chunkParser
*/
block createBlock(int x, int y, int z, unsigned int* blockStates, section parentSection);

/*!
 @brief Returns an array of strings containing a complete total block palette
 @details The only memory it allocates is the returned array
 @param sections The sections to create the global palette from
 @param len The length of the sections array
 @param outLen The length of the returned array
 @param freeSectionPalettes Whether to free the section palettes
 @return The global palette
 @ingroup chunkParser
*/
char** createGlobalPalette(section* sections, int len, int* outLen, bool freeSectionPalettes);

/*!
 @brief Frees all the allocated memory by getSections
 @param sections The sections to free
 @param sectionLen The length of the sections array
 @ingroup chunkParser
*/
void freeSections(section* sections, int sectionLen);