#!/bin/bash
x86_64-w64-mingw32-gcc-win32 buffer.c -o ../buffer.ow -c
x86_64-w64-mingw32-gcc-win32 nbt_parsing.c -o ../nbt_parsing.ow -c 
x86_64-w64-mingw32-gcc-win32 nbt_treeops.c -o ../nbt_treeops.ow -c
x86_64-w64-mingw32-gcc-win32 nbt_util.c -o ../nbt_util.ow -c 