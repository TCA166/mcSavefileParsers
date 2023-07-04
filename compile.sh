#!/bin/bash
#apt install gcc
if ! test -f "buffer.o"; then
    cd ./cNBT
    ./cNBTcompile.sh "$@"
    cd ../
fi
gcc regionParser.c -o regionParser.o -c "$@" #our region file handling lib
gcc chunkParser.c -o chunkParser.o "$@" -c -lm #our chunk file handling lib
gcc hTable.c -o hTable.o "$@" -c #our hash table library
gcc model.c -o model.o "$@" -c #our 3d model handling lib
gcc regionFileReader.c regionParser.o -o regionFileReader -lz -static "$@" #utility 1
gcc chunkExtractor.c regionParser.o -o chunkExtractor -lz -lm -static "$@" #utility 2
gcc modelGenerator.c model.o hTable.o chunkParser.o nbt_parsing.o nbt_treeops.o buffer.o nbt_util.o -o modelGenerator -lm "$@" # utility 3