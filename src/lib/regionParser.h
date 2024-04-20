/*!
 @file regionParser.h
 @brief A parser for region files
*/

#ifndef REGIONPARSER_H

/*!
 @defgroup regionParser Region Parser
 @brief A parser for region files
*/

/*!
 @def chunkN
 @brief How many chunks are in a region
 @ingroup regionParser
*/
#define chunkN 1024


//How to translate the compression byte

/*!
 @enum compression_t
 @brief Minecraft indicator for how the chunk is compressed
 @ingroup regionParser
*/
typedef enum{
    GZip = 1,
    Zlib = 2,
    None = 3
} compression_t;

/*!
 @def chunkIsNull
 @brief If the chunk offset is 0 and sector count is 0 then the chunk is considered not generated and empty so basically NULL
 @param chunk The chunk to check
 @ingroup regionParser
*/
#define chunkIsNull(chunk) (chunk.offset == 0 && chunk.sectorCount == 0)

/*!
 @def coordsToIndex
 @brief Converts x and z coordinates to a 1D index
 @param x The x coordinate
 @param z The z coordinate
 @ingroup regionParser
*/
#define coordsToIndex(x, z) ((x & 31) + (z & 31) * 32)

/*!
 @def coordsToOffset
 @brief Converts x and z coordinates to a 1D offset
 @param x The x coordinate
 @param z The z coordinate
 @ingroup regionParser
*/
#define coordsToOffset(x, z) 4 * coordsToIndex(x, z)

/*!
 @typedef byte
 @brief A byte type
 @note This is a typedef for unsigned char
 @ingroup regionParser
*/
typedef unsigned char byte;

/*!
 @struct chunk
 @brief A chunk structure
 @ingroup regionParser
*/
typedef struct{
    /*!
     @var x
     @brief the x chunk coordinate
    */
    int x;
    /*!
     @var z
     @brief the y chunk coordinate
    */
    int z;
    /*!
     @var offset
     @brief the offset within the region file
     @note this isn't particularly useful
    */
    unsigned int offset; //3 bytes of offset
    /*!
     @var sectorCount
     @brief a single byte indicating how many region file sectors this chunk takes up
     @note this isn't particularly useful
    */
    byte sectorCount; //1 byte indicating chunk data length
    /*!
     @var timestamp
     @brief the minecraft timestamp
    */
    unsigned int timestamp;
    /*!
     @var byteLength
     @brief length of region in bytes
    */
    int byteLength; //it actually is signed in the files
    /*!
     @var compression
     @brief enum value indicating compression type
     @note this isn't particularly useful
    */
    compression_t compression;
    /*!
     @var data
     @brief decompressed data
    */
    byte* data; //pointer to decompressed bytes
} chunk;

/*!
 @brief Returns a chunk based on coordinates.
 @note In case of a non critical parsing error (I/O errors will still halt execution immediately) the function will return a non complete object.
 @note So if byteLength is 0 then it couldn't read the bytes section of the region file etc... 
 @param x The x coordinate of the chunk
 @param z The z coordinate of the chunk
 @param regionFile The region file to read from
 @param regionFileName The name of the region file
 @return The chunk object
*/
chunk getChunk(int x, int z, FILE* regionFile, char* regionFileName);

/*!
 @brief Wrapper for regionFileReader
 @details Extracts all the chunks in regionFile.
 @return Returns a dynamic array of chunks with 1024 chunks.
 @note High-er level function that does error handling on it's own.
 @param regionFile The region file to read from
 @ingroup regionParser
*/
chunk* getChunks(FILE* regionFile);

/*!
 @brief Wrapper for chunkExtractor
 @note High-er level function that does error handling on it's own
 @param regionDirPath The path to the region directory
 @param x The x coordinate of the chunk
 @param z The z coordinate of the chunk
 @return The chunk object
 @ingroup regionParser
*/
chunk extractChunk(char* regionDirPath, int x, int z);

#endif
