#!/bin/bash
#apt install gcc-mingw-w64
#apt install libz-mingw-w64-dev
x86_64-w64-mingw32-gcc-win32  regionParser.c -o regionParser.o -c
x86_64-w64-mingw32-gcc-win32  chunkParser.c -o chunkParser.o -c -lm
x86_64-w64-mingw32-gcc-win32  model.c -o model.o -c
x86_64-w64-mingw32-gcc-win32  regionFileReader.c regionParser.o -o regionFileReader.exe -lz
x86_64-w64-mingw32-gcc-win32  chunkExtractor.c regionParser.o -o chunkExtractor.exe -lz -lm
x86_64-w64-mingw32-gcc-win32  modelGenerator.c model.o chunkParser.o nbt_parsing.o nbt_treeops.o buffer.o nbt_util.o -o modelGenerator -lz -lm