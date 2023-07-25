
//How many chunks are in a region
#define chunkN 1024

//Length in bytes of savefile segment 
#define segmentLength 4096

//How to translate the compression byte

#define GZip 1
#define Zlib 2
#define Uncompressed 3

//If the chunk offset is 0 and sector count is 0 then the chunk is considered not generated and empty so basically NULL
#define chunkIsNull(chunk) (chunk.offset == 0 && chunk.sectorCount == 0)

#define coordsToIndex(x, z) ((x & 31) + (z & 31) * 32)

#define coordsToOffset(x, z) 4 * coordsToIndex(x, z)

//Unsigned char
typedef unsigned char byte;

//A struct representing a mc game chunk
typedef struct chunk{
    int x; //x coordinate in chunk coordinates
    int z; //z coordinate in chunk coordinates
    unsigned int offset; //3 bytes of offset
    byte sectorCount; //1 byte indicating chunk data length
    unsigned int timestamp;
    int byteLength; //it actually is signed in the files
    byte compression;
    byte* data; //pointer to decompressed bytes
} chunk;

/*
Returns a chunk based on coordinates.
In case of a non critical parsing error (I/O errors will still halt execution immediately) the function will return a non complete object.
So if byteLength is 0 then it couldn't read the bytes section of the region file etc... 
*/
chunk getChunk(int x, int z, FILE* regionFile, char* regionFileName);

/*
Wrapper for regionFileReader
Extracts all the chunks in regionFile.
Returns a dynamic array of chunks with 1024 chunks.
High-er level function that does error handling on it's own.
*/
chunk* getChunks(FILE* regionFile);

/*
Wrapper for chunkExtractor
High-er level function that does error handling on it's own
*/
chunk extractChunk(char* regionDirPath, int x, int z);
