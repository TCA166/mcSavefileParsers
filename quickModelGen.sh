#!/bin/bash
./chunkExtractor $1 $2 $3
./modelGenerator ./$1.$2.nbt -m out.mtl -b