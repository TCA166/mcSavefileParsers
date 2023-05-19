#!/bin/bash
#apt install gcc
gcc regionParser.c -o regionParser.o -c -g
gcc regionFileReader.c regionParser.o -o regionFileReader -lz -g
gcc chunkExtractor.c regionParser.o -o chunkExtractor -lz -lm -g
gcc modelGenerator.c nbt_parsing.o nbt_treeops.o buffer.o nbt_util.o -o modelGenerator -lz -lm -g