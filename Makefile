
cNBT.o: 
	gcc cNBT/buffer.c -o cNBT/buffer.o -c $(FLAGS)
	gcc cNBT/nbt_parsing.c -o cNBT/nbt_parsing.o -c $(FLAGS)
	gcc cNBT/nbt_treeops.c -o cNBT/nbt_treeops.o -c $(FLAGS)
	gcc cNBT/nbt_util.c -o cNBT/nbt_util.o -c $(FLAGS)
	ld -relocatable cNBT/buffer.o cNBT/nbt_parsing.o cNBT/nbt_treeops.o cNBT/nbt_util.o -o cNBT.o

regionParser.o:
	gcc regionParser.c -o regionParser.o -c -lm $(FLAGS)

chunkParser.o:
	gcc chunkParser.c -o chunkParser.o -c -lm $(FLAGS)

hTable.o:
	gcc hTable.c -o hTable.o -c $(FLAGS)

model.o:
	gcc model.c -o model.o -c $(FLAGS)

regionFileReader: regionParser.o
	gcc regionFileReader.c regionParser.o -o regionFileReader -lz -lm $(FLAGS)

chunkExtractor: regionParser.o
	gcc chunkExtractor.c regionParser.o -o chunkExtractor -lz -lm $(FLAGS)

generator.o:
	gcc generator.c -o generator.o -c $(FLAGS)

modelGenerator: model.o generator.o hTable.o chunkParser.o cNBT.o
	gcc modelGenerator.c generator.o model.o hTable.o chunkParser.o cNBT.o -o modelGenerator -lm $(FLAGS)

radiusGenerator: model.o generator.o hTable.o chunkParser.o regionParser.o cNBT.o
	gcc radiusGenerator.c generator.o model.o regionParser.o hTable.o chunkParser.o cNBT.o -lz -lm -o radiusGenerator $(FLAGS)

all: radiusGenerator modelGenerator chunkExtractor regionFileReader

cNBT.ow:
	x86_64-w64-mingw32-gcc-win32 cNBT/buffer.c -o cNBT/buffer.ow -c $(FLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_parsing.c -o cNBT/nbt_parsing.ow -c $(FLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_treeops.c -o cNBT/nbt_treeops.ow -c $(FLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_util.c -o cNBT/nbt_util.ow -c $(FLAGS)
	x86_64-w64-mingw32-ld -relocatable cNBT/buffer.ow cNBT/nbt_parsing.ow cNBT/nbt_treeops.ow cNBT/nbt_util.ow -o cNBT.ow

regionParser.ow:
	x86_64-w64-mingw32-gcc-win32 regionParser.c -o regionParser.ow -c $(FLAGS)

chunkParser.ow:
	x86_64-w64-mingw32-gcc-win32 chunkParser.c -o chunkParser.ow -c -lm $(FLAGS)

hTable.ow:
	x86_64-w64-mingw32-gcc-win32 hTable.c -o hTable.ow -c $(FLAGS)

model.ow:
	x86_64-w64-mingw32-gcc-win32 model.c -o model.ow -c $(FLAGS)

regionFileReader.exe: regionParser.ow
	x86_64-w64-mingw32-gcc-win32 regionFileReader.c regionParser.ow -o regionFileReader.exe -lz -static $(FLAGS)

chunkExtractor.exe: regionParser.ow
	x86_64-w64-mingw32-gcc-win32 chunkExtractor.c regionParser.ow -o chunkExtractor.exe -lz -lm -static $(FLAGS)

generator.ow:
	x86_64-w64-mingw32-gcc-win32 generator.c -o generator.ow -c $(FLAGS)

modelGenerator.exe: generator.ow model.ow chunkParser.ow hTable.ow cNBT.ow
	x86_64-w64-mingw32-gcc-win32 modelGenerator.c generator.ow model.ow chunkParser.ow hTable.ow cNBT.ow -o modelGenerator.exe -lm $(FLAGS)

windows: modelGenerator.exe chunkExtractor.exe regionFileReader.exe

winBuildPrerequisites:
	sudo apt-get install mingw-w64-common mingw-w64-x86-64-dev binutils-mingw-w64-x86-64 libz-mingw-w64-dev

clean:
	rm -f *.o
	rm -f cNBT/*.o
	rm -f *.ow
	rm -f cNBT/*.ow