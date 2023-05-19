#!/bin/bash
gcc buffer.c -o ../buffer.o -c $1
gcc nbt_parsing.c -o ../nbt_parsing.o -c $1
gcc nbt_treeops.c -o ../nbt_treeops.o -c $1
gcc nbt_util.c -o ../nbt_util.o -c $1