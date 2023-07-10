#!/bin/bash
./regionFileReader ./region/r.0.0.mca ./test
./chunkExtractor ./region 0 0
./modelGenerator ./0.0.nbt -m out.mtl -b -o obj.obj
python mtlGen.py ./block -s -t