#!/bin/bash
gcc -Wall -O -fanalyzer r src/lib/*.c src/lib/*.h src/*.c -o /dev/null &> staticAnalysis.log