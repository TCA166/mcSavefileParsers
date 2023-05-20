#!/bin/bash
./regionFileReader ./region/r.0.0.mca ./test
./chunkExtractor ./region 0 0
./modelGenerator ./0.0.nbt