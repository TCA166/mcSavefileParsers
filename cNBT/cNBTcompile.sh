#!/bin/bash
gcc buffer.c -o ../buffer.o -c
gcc nbt_parsing.c -o ../nbt_parsing.o -c
gcc nbt_treeops.c -o ../nbt_treeops.o -c
gcc nbt_util.c -o ../nbt_util.o -c