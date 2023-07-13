#!/bin/bash
#apt install gcc-mingw-w64
#apt install libz-mingw-w64-dev
if ! test -f "buffer.ow"; then
    cd ./cNBT
    ./cNBTcompileWin.sh
    cd ../
fi
#everything where there is -lz i add -static because zlib isn't usually on windows... at least it wasn't on mine
x86_64-w64-mingw32-gcc-win32 regionParser.c -o regionParser.ow -c -O3
x86_64-w64-mingw32-gcc-win32 chunkParser.c -o chunkParser.ow -c -lm -O3
x86_64-w64-mingw32-gcc-win32 hTable.c -o hTable.ow -c -O3
x86_64-w64-mingw32-gcc-win32 model.c -o model.ow -c -O3
x86_64-w64-mingw32-gcc-win32 regionFileReader.c regionParser.ow -o regionFileReader.exe -lz -static -O3
x86_64-w64-mingw32-gcc-win32 generator.c -o generator.ow -c -O3
x86_64-w64-mingw32-gcc-win32 chunkExtractor.c regionParser.ow -o chunkExtractor.exe -lz -lm -static -O3
x86_64-w64-mingw32-gcc-win32 modelGenerator.c generator.ow model.ow chunkParser.ow hTable.ow nbt_parsing.ow nbt_treeops.ow buffer.ow nbt_util.ow -o modelGenerator.exe -lm -O3
#x86_64-w64-mingw32-gcc-win32 radiusGenerator.c generator.ow model.ow regionParser.ow hTable.ow chunkParser.ow nbt_parsing.ow nbt_treeops.ow buffer.ow nbt_util.ow -o radiusGenerator.exe -lm -lz -static -O3