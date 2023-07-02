#!/bin/bash
python3 objGen.py $1
python3 mtlGen.py $1/textures/block -s -c -a