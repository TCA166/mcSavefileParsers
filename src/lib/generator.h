/*!
 @file generator.h
 @brief Contains functions for generating 3d models from nbt data
 @details This file contains functions for generating 3d models from nbt data
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>
#include "model.h"
#include "chunkParser.h"

/*!
 @defgroup generator Model Generator
 @brief Functions for generating 3d models from nbt data
*/

//moved this here from model.c in order to properly separate that file into a proper 3d model rendering lib and a chunk nbt handling lib

/*!
 @brief Creates a new cube object based on a block object.
 @param block The block object to create the cube from
 @param side The side of the block to create the cube from
 @param material The material to use for the cube
 @return The created cube object
 @ingroup generator
*/
cube cubeFromBlock(block block, const unsigned int side, material* material);

/*!
 @brief Creates a new cube based model object from a section array.
 @param sections The array of sections to create the model from
 @param sectionLen The length of the sections array
 @param materials The materials hash table to use for the model
 @param yLim Whether to limit the model to a certain vertical range
 @param upLim The upper limit of the vertical range
 @param downLim The lower limit of the vertical range
 @param side The side of the block to render
 @param matCheck Whether to check for materials
 @param xOff The x offset of the model
 @param zOff The z offset of the model
 @return The created cube model object
 @ingroup generator
*/
cubeModel createCubeModel(section* sections, int sectionLen, hashTable* materials, bool yLim, int upLim, int downLim, unsigned int side, bool matCheck, int xOff, int zOff);

//Generally speaking i could just pass a whole chunk object from regionParser here, but I like the design of having that be completely separate

/*!
 @brief Generates a model object from binary nbt data.
 @details materials and objects can be NULL. Consumes data, materials and objects entirely.
 @param data The binary nbt data
 @param dataSize The size of the binary nbt data
 @param materials The materials hash table to use for the model
 @param objects The objects hash table to use for the model
 @param yLim Whether to limit the model to a certain vertical range
 @param upLim The upper limit of the vertical range
 @param downLim The lower limit of the vertical range
 @param b Whether to render chunk borders
 @param f Whether to disable face culling
 @param side The side of the block to render
 @param chunkX The x coordinate of the chunk
 @param chunkZ The z coordinate of the chunk
 @return The generated model object
 @ingroup generator
*/
model generateFromNbt(unsigned char* data, int dataSize, hashTable* materials, hashTable* objects, bool yLim, int upLim, int downLim, bool b, bool f, unsigned int side, int chunkX, int chunkZ);

/*!
 @brief Frees the objects within a hashTable
 @param objects The hashTable to free
 @ingroup generator
*/
void freeObjectsHashTable(hashTable* objects);

#endif
