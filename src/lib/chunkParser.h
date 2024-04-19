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
 @details The section structure contains the block data, the block palette, the y coordinate of the section and the lengths of the block data and palette
 @note This struct is mainly used for holding the raw nbt data of a section
 @todo Add biomes
 @see getBlockStates
 @ingroup chunkParser
*/
typedef struct{
    //we ignore the biomes because we don't need that data
    /*! 
     @var blockData
     @brief raw nbt section block data
     @details Packed array with padding, so that the individual block data isn't split.
     @details The length of one element in this packed array is 4 or log2(paletteLen) whichever is higher.
     @details the individual values stored here point to the identifiers in blockPalette
     @warning can be NULL, in which case the section is full of the same block type: blockPalette[0]
    */
    uint64_t* blockData; 
    /*!
     @var blockPalette
     @brief A string array of block identifiers
    */
    char** blockPalette;
    /*!
     @var blockDataLen
     @brief Length of the blockData long array
    */
    size_t blockDataLen;
    /*!
     @var paletteLen
     @brief Length of the blockPalette array
    */
    size_t paletteLen;
    /*!
     @var y
     @brief the y index of this section. They go from -4 to whatever the height limit of the world is but divided by 16
    */
    short y;
} section;

/*!
 @struct block
 @brief A block structure with it's own coordinates and a string containing it's type
 @ingroup chunkParser
*/
typedef struct{
    /*!
     @var x
     @brief The x position of the block
    */
    int x;
    /*!
     @var y
     @brief The y position of the block
    */
    int y;
    /*!
     @var z
     @brief The z position of the block
    */
    int z;
    /*!
     @var type
     @brief A string block type identifier
     @note Shouldn't be NULL
    */
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