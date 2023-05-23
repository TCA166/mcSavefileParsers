#!/bin/bash
#apt install gcc
if ! test -f "buffer.o"; then
    cd ./cNBT
    ./cNBTcompile.sh $1
    cd ../
fi
gcc regionParser.c -o regionParser.o -c $1 #our region file handling lib
gcc chunkParser.c -o chunkParser.o $1 -c #our chunk file handling lib
gcc model.c -o model.o $1 -c #our 3d model handling lib
gcc regionFileReader.c regionParser.o -o regionFileReader -lz $1 #utility 1
gcc chunkExtractor.c regionParser.o -o chunkExtractor -lz -lm $1 #utility 2
gcc modelGenerator.c model.o chunkParser.o nbt_parsing.o nbt_treeops.o buffer.o nbt_util.o -o modelGenerator -lz -lm $1 # utility 3