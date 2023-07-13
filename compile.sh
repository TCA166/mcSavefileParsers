#!/bin/bash
#apt install gcc
#Cmake? never heard of it :)
if ! test -f "buffer.o"; then
    cd ./cNBT
    ./cNBTcompile.sh "$@"
    cd ../
fi
gcc regionParser.c -o regionParser.o -c -lm "$@" #our region file handling lib
gcc chunkParser.c -o chunkParser.o "$@" -c -lm #our chunk file handling lib
gcc hTable.c -o hTable.o "$@" -c #our hash table library
gcc model.c -o model.o "$@" -c #our 3d model handling lib
gcc regionFileReader.c regionParser.o -o regionFileReader -lz -lm "$@" #utility 1
gcc chunkExtractor.c regionParser.o -o chunkExtractor -lz -lm "$@" #utility 2
gcc generator.c -o generator.o -c "$@"
gcc modelGenerator.c generator.o model.o hTable.o chunkParser.o nbt_parsing.o nbt_treeops.o buffer.o nbt_util.o -o modelGenerator -lm "$@" # utility 3
gcc radiusGenerator.c generator.o model.o regionParser.o hTable.o chunkParser.o nbt_parsing.o nbt_treeops.o buffer.o nbt_util.o -lz -lm "$@" -o radiusGenerator