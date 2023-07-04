
//How many chunks are in a region
#define chunkN 1024

//Length in bytes of savefile segment 
#define segmentLength 4096

//How to translate the compression byte

#define GZip 1
#define Zlib 2
#define Uncompressed 3

#define getRegion(coord) coord>>5

#define coordsToOffset(x, z) 4 * ((x & 31) + (z & 31) * 32)

//Unsigned char
typedef unsigned char byte;

//A struct representing a mc game chunk
struct chunk{
    int x; //x coordinate in chunk coordinates
    int z; //z coordinate in chunk coordinates
    unsigned int offset; //3 bytes of offset
    byte sectorCount; //1 byte indicating chunk data length
    unsigned int timestamp;
    unsigned int byteLength;
    byte compression;
    byte* data; //pointer to decompressed bytes
};

//struct chunk
typedef struct chunk chunk;

//Returns a chunk based on coordinates
chunk getChunk(int x, int z, FILE* regionFile);

//Extracts payload data about thisChunk from regionFile and appends that data to thisChunk
int getChunkData(chunk* thisChunk, FILE* regionFile);

/*Extracts all the chunks in regionFile.
Returns a dynamic array of chunks with 1024 chunks.*/
chunk* getChunks(FILE* regionFile);