#!/bin/bash
python3 ./src/scripts/objGen.py $1
python3 ./src/scripts/mtlGen.py $1/textures/block -s -c -a