#!/bin/bash
gcc mc.c -o mc.o -c
gcc regionFileReader.c mc.o -o regionFileReader -lz
gcc chunkExtractor.c mc.o -o chunkExtractor -lz -lm