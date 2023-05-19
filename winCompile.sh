#!/bin/bash
#apt install gcc-mingw-w64
#apt install libz-mingw-w64-dev
x86_64-w64-mingw32-gcc-win32  mc.c -o mc.o -c
x86_64-w64-mingw32-gcc-win32  regionFileReader.c mc.o -o regionFileReader.exe -lz
x86_64-w64-mingw32-gcc-win32  chunkExtractor.c mc.o -o chunkExtractor.exe -lz -lm