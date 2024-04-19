/*!
 @file model.h
 @brief Definitions for 3D model structs and functions
*/

/*!
 @defgroup model 3D Model
 @brief Definitions for 3D model structs and functions
*/

#include "hTable.h"
#include <stdbool.h>

/*!
 @struct material
 @brief A material struct that can be used in a model
 @todo add all the wavefront material properties
 @see model
 @ingroup model
*/
typedef struct{
    /*!
     @var name
     @brief the material name
    */
    char* name;
    //here i could add more fields... but field d is the only one we need
    /*!
     @var d
     @brief the material density
    */
    float d;
} material;

/*!
 @union vertex
 @brief A 3D vertex struct holding three floats
 @see cubeFace
 @see object
 @ingroup model
*/
typedef union {
    /*!
     @var coordinates
     @brief a look into the vertex coordinates
    */
    struct{
        /*!
        @var x
        @brief the x position of the vertices
        */
        float x;
        /*!
        @var y
        @brief the y position of the vertices
        */
        float y;
        /*!
        @var z
        @brief the z position of the vertices
        */
        float z;
    } coordinates;
    /*!
     @var arr
     @brief a look into the vertex as an array of floats
    */
    float arr[3];
} vertex;

/*!
 @union cubeFace
 @brief A face of a cube containing pointers to vertices
 @details A face with only 4 vertices. V variables point to vertices in the associated cube object
 @see cube
 @ingroup model
*/
typedef union{
    /*!
     @var vertices
     @brief a look into the cubeFace as individual integers
    */
    struct{
        unsigned int v1;
        unsigned int v2;
        unsigned int v3;
        unsigned int v4;
    } vertices;
    /*!
     @var arr
     @brief a look at a cubeFace as an array of pointers
    */
    unsigned int arr[4];
} cubeFace;

/*!
 @struct cube
 @brief obj file cube with 6 cubeFaces and 8 defining vertices
 @details A static array of vertices with set positions define the shape, while a static array of faces point to these vertices giving information on how walls look like
 @see cubeModel
 @ingroup model
*/
typedef struct{
    /*!
     @var x
     @brief the x position of the cube
    */
    float x;
    /*!
     @var y
     @brief the y position
    */
    float y;
    /*!
     @var z
     @brief the z position
    */
    float z;
    /*!
     @var side
     @brief length of the cube side
    */
    float side; //Cannot be lower than 0 for very obvious reasons
    /*!
     @var vertices
     @brief array of vertices relative to the cube
    */
    vertex vertices[8];
    /*!
     @var faces
     @brief nullable array of faces
    */
    cubeFace* faces[6];
    /*!
     @var type
     @brief the type of the cube, usually Minecraft material
     @note may be NULL
    */
    char* type;
    /*!
     @var m
     @brief the material associated with the cube
     @note may be NULL
    */
    material* m;
} cube;

/*!
 @struct cubeModel
 @brief A strict 3D model struct that is made up of cubes in a strict 3D arrangement.
 @details A cubeModel consists of a nullable 3d array of cubes that transforms a 3d position aligned to 1 into a cube*
 @note Can be transformed into a normal loose model.
 @note Ideally if you plan on generating a model of something that can be represented by cubes you first generate this.
 @ingroup model
*/
typedef struct{
    /*!
     @var x
     @brief the maximum x index, or the size of the allocated x dimension
    */
    unsigned int x;
    /*!
     @var y
     @brief the maximum y index, or the size of the allocated y dimension
    */
    unsigned int y;
    /*!
     @var z
     @brief the maximum z index, or the size of the allocated z dimension
    */
    unsigned int z;
    /*!
     @var cubes
     @brief nullable 3d array of cubes
    */
    cube**** cubes;
} cubeModel;

/*!
 @struct objFace
 @brief A dynamic array of vertices - an object face
 @details A dynamic array of vertices - an object face. The vertices are relative to the object's x, y, z coordinates.
 @see object
 @ingroup model
*/
typedef struct{
    /*!
     @var vertexCount
     @brief the number of vertices
    */
    size_t vertexCount;
    /*!
     @var vertices
     @brief an allocated array of vertices
    */
    int* vertices;
    /*!
     @var m
     @brief NULL or pointer to the material
    */
    material* m;
} objFace;

/*!
 @struct object
 @brief A 3D object struct that is made up of vertices and faces.
 @details A 3D object struct that is made up of vertices and faces. The vertices are relative to the object's x, y, z coordinates.
 @see model
 @ingroup model
*/
typedef struct{
    /*!
     @var x
     @brief the x position of the object
    */
    float x;
    /*!
     @var y
     @brief the y position of the object
    */
    float y;
    /*!
     @var z
     @brief the z position
    */
    float z;
    /*!
     @var vertexCount
     @brief the number of vertices this object uses
    */
    size_t vertexCount;
    /*!
     @var vertices
     @brief an array of vertices this object uses
    */
    vertex* vertices;
    /*!
     @var faceCount
     @brief the number of faces in this model
    */
    size_t faceCount;
    /*!
     @var faces
     @brief an array of faces
    */
    objFace* faces;
    /*!
     @var m
     @brief the material pointer
    */
    material* m;
    /*!
     @var type
     @brief the type of the object, generally a Minecraft identifier
    */
    char* type;
} object;

//The idea behind the materialArr is that it's way more memory efficient to store materials once rather than possibly hundreds of times

/*!
 @struct model
 @brief A loose array of even looser objects with an associated array of materials that objects can point to.
 @details A loose set of objects located in an undefined space, with materials pointing to materialArr.
 @warning Having objects not using materialArr might lead to nasty memory leaks
 @ingroup model
*/
typedef struct{
    /*!
     @var objects
     @brief a nullable list of objects
    */
    object** objects; //Nullable array of objects
    /*!
     @var objectCount
     @brief the size of the objects array
    */
    size_t objectCount;
    /*!
     @var materialArr
     @brief Optional array of materials, that objects can point to
    */
    material** materialArr; 
    /*!
     @var materialCount
     @brief Size of materialArr
    */
    size_t materialCount;
} model;

/*!
 @def dimensionalFor
 @brief A 3D model struct that is made up of vertices and faces.
 @details A 3D model struct that is made up of vertices and faces. The vertices are relative to the model's x, y, z coordinates.
 @param xLim The maximum x index
 @param yLim The maximum y index
 @param zLim The maximum z index
 @ingroup model
*/
#define dimensionalFor(xLim, yLim, zLim) \
    for(int x = 0; x < xLim; x++) \
    for(int y = 0; y < yLim; y++) \
    for(int z = 0; z < zLim; z++)

/*!
 @def foreachObject
 @brief Foreach object in a model*. Provides index variable o, and assures that the object will not be null
 @param model The model to iterate over
 @ingroup model
*/
#define foreachObject(model) \
    for(int o = 0; o < model->objectCount; o++) \
    if(model->objects[o] != NULL)

/*!
 @def foreachCube
 @brief Foreach cube in a cubeModel*. Provides x, y and z, and assures that the cube will not be null
 @param cubeModel The cubeModel to iterate over
 @ingroup model
*/
#define foreachCube(cubeModel) \
    dimensionalFor(cubeModel->x, cubeModel->y, cubeModel->z) \
    if(cubeModel->cubes[x][y][z] != NULL)

/*! 
 @typedef offset_t
 @brief Type for holding vertex offset (unsigned long)
 @ingroup model
*/
typedef unsigned long offset_t;

/*!
 @brief Initialises a model variable and allocates necessary memory.
 @param objectCount The number of objects in the model
 @return a new reference that will need to be freed.
 @ingroup model
*/
model initModel(int objectCount);

/*!
 @brief Initialises a new cubeModel variable with dimensions x*y*z cubes and allocates necessary memory.
 @param x The x dimension of the cubeModel
 @param y The y dimension of the cubeModel
 @param z The z dimension of the cubeModel
 @return Returns a new reference that will need to be freed.
 @ingroup model
*/
cubeModel initCubeModel(int x, int y, int z);

/*!
 @brief Creates a new vertex
 @param x The x coordinate of the vertex
 @param y The y coordinate of the vertex
 @param z The z coordinate of the vertex
 @return The created vertex
 @ingroup model
*/
vertex newVertex(int x, int y, int z);

/*!
 @brief Removes all outward or internal faces from a model.
 @details All cubes that are NULL or whose type is in ignoreType are considered to be outward and faces touching those cubes are not culled.
 @note Culled faces are free'd and set as null.
 @param thisModel The model to cull faces from
 @param cullChunkBorder Whether to cull faces on the chunk border
 @param specialObjects A hash table of special objects to not cull faces from
 @return The number of faces culled
 @ingroup model
*/
unsigned int cullFaces(cubeModel* thisModel, bool cullChunkBorder, hashTable* specialObjects);

/*!
 @brief Converts a cubeModel to a normal model, and inserts special objects if specialObjects!=NULL when types match.
 @details Uses initModel to create a new model reference, and also creates new object references within the new model.
 @param m The cubeModel to convert
 @param specialObjects A hash table of special objects to insert into the model
 @return The new model reference
 @ingroup model
*/
model cubeModelToModel(const cubeModel* m, hashTable* specialObjects);

/*!
 @brief Generates a model from a cubeModel.
 @details Generates a model from a cubeModel. The model will have the same dimensions as the cubeModel.
 @note Returns a string that are valid .obj file contents. Be sure to free the contents once you are done with them.
 @note outSize, materialFileName and offset can be NULL, if you wish to not get those values back.
 @note If materialFileName is NULL the mttlib line will not be added, but usemtl will be generated normally.
 @param thisModel The model to generate
 @param outSize The size of the generated model
 @param materialFileName The name of the material file
 @param offset The offset of the model
 @return The generated model
 @ingroup model
*/
char* generateModel(const model* thisModel, size_t* outSize, const char* materialFileName, offset_t* offset);

/*!
 @brief Frees all cube faces, cubes and the model itself. Doesn't free the material.
 @param m The cubeModel to free
 @ingroup model
*/
void freeCubeModel(cubeModel* m);

/*!
 @brief Frees everything that can be allocated in a model m. This includes the materials
 @param m The model to free
 @ingroup model
*/
void freeModel(model* m);

/*!
 @brief Creates and allocates necessary memory for a new cubeFace object
 @param a The first vertex index
 @param b The second vertex index
 @param c The third vertex index
 @param d The fourth vertex index
 @return The new cubeFace reference
 @ingroup model
*/
cubeFace* newCubeFace(int a, int b, int c, int d);

/*!
 @brief Extracts materials from a mtl file.
 @details Allocates necessary memory for material type strings.
 @param filename The name of the mtl file
 @return Returns a new reference to a material hash table.
 @ingroup model
*/
hashTable* getMaterials(char* filename);

/*!
 @brief Creates a clone of a material struct in memory
 @param input The material to clone
 @return The cloned material
 @ingroup model
*/
material* cloneMaterial(const material* input);

/*!
 @brief Converts a cubeFace to the dynamic obj face.
 @param face the face to convert
 @return The converted face
 @ingroup model
*/
objFace deCube(cubeFace face);

/*!
 @brief Converts a cube into an object.
 @details Allocates memory for faces, vertices and type.
 @note Utilizes deCube for face conversion so by proxy allocates memory for face vertices.
 @param c The cube to convert
 @return The converted object
 @ingroup model
*/
object deCubeObject(cube* c);

/*!
 @brief Converts a cubeModel to a model.
 @details Returns a hash table with objects from a wavefront file. Returns NULL if fails.
 @param filename The name of the wavefront file
 @param materials The materials hash table to use for the model
 @param side The side of the block to render
 @return The hash table of objects
 @ingroup model
*/
hashTable* readWavefront(char* filename, hashTable* materials, unsigned int side);

/*!
 @brief Returns the distance between two vectors
 @param a The first vector
 @param b The second vector
 @return The distance between the two vectors
 @ingroup model
*/
double distanceBetweenVectors(vertex a, vertex b);

/*!
 @brief Returns true if two vertices are equal
 @param a The first vertex
 @param b The second vertex
 @return True if the vertices are equal
 @ingroup model
*/
bool verticesEqual(vertex a, vertex b);

/*!
 @brief Returns the number of vertices in the model. 
 @note Should be equivalent to the offset value if the model were to be generated -1
 @param m The model to count the vertices of
 @return The number of vertices in the model
 @ingroup model
*/
offset_t getTotalVertexCount(model m);

/*!
 @brief Returns a generic cube object
 @param side The side of the cube
 @return The generic cube object
 @ingroup model
*/
cube createGenericCube(unsigned int side);

/*!
 @brief Converts a model to a single well optimized object.
 @note Removes overlapping vertices.
 @note type can be null, but shouldn't be free'd as it's assigned to the result.
 @param m The model to convert
 @param type The type of the object
 @return The optimized object
 @ingroup model
*/
object modelToObject(const model* m, const char* type);
