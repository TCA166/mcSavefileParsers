
all: radiusGenerator modelGenerator chunkExtractor regionFileReader

cNBT.o: 
	gcc cNBT/buffer.c -o cNBT/buffer.o -c $(FLAGS)
	gcc cNBT/nbt_parsing.c -o cNBT/nbt_parsing.o -c $(FLAGS)
	gcc cNBT/nbt_treeops.c -o cNBT/nbt_treeops.o -c $(FLAGS)
	gcc cNBT/nbt_util.c -o cNBT/nbt_util.o -c $(FLAGS)
	ld -relocatable cNBT/buffer.o cNBT/nbt_parsing.o cNBT/nbt_treeops.o cNBT/nbt_util.o -o cNBT.o

regionParser.o: regionParser.c
	gcc regionParser.c -o regionParser.o -c -lm $(FLAGS)

chunkParser.o: chunkParser.c
	gcc chunkParser.c -o chunkParser.o -c -lm $(FLAGS)

hTable.o: hTable.c
	gcc hTable.c -o hTable.o -c $(FLAGS)

model.o: model.c
	gcc model.c -o model.o -c $(FLAGS)

regionFileReader: regionParser.o regionFileReader.c
	gcc regionFileReader.c regionParser.o -o regionFileReader -lz -lm $(FLAGS)

chunkExtractor: regionParser.o chunkExtractor.c
	gcc chunkExtractor.c regionParser.o -o chunkExtractor -lz -lm $(FLAGS)

generator.o: generator.c
	gcc generator.c -o generator.o -c $(FLAGS)

modelGenerator: model.o generator.o hTable.o chunkParser.o cNBT.o modelGenerator.c
	gcc modelGenerator.c generator.o model.o hTable.o chunkParser.o cNBT.o -o modelGenerator -lm $(FLAGS)

radiusGenerator: model.o generator.o hTable.o chunkParser.o regionParser.o cNBT.o radiusGenerator.c
	gcc radiusGenerator.c generator.o model.o regionParser.o hTable.o chunkParser.o cNBT.o -lz -lm -o radiusGenerator $(FLAGS)

cNBT.ow:
	x86_64-w64-mingw32-gcc-win32 cNBT/buffer.c -o cNBT/buffer.ow -c $(FLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_parsing.c -o cNBT/nbt_parsing.ow -c $(FLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_treeops.c -o cNBT/nbt_treeops.ow -c $(FLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_util.c -o cNBT/nbt_util.ow -c $(FLAGS)
	x86_64-w64-mingw32-ld -relocatable cNBT/buffer.ow cNBT/nbt_parsing.ow cNBT/nbt_treeops.ow cNBT/nbt_util.ow -o cNBT.ow

regionParser.ow: regionParser.c
	x86_64-w64-mingw32-gcc-win32 regionParser.c -o regionParser.ow -c $(FLAGS)

chunkParser.ow: chunkParser.c
	x86_64-w64-mingw32-gcc-win32 chunkParser.c -o chunkParser.ow -c -lm $(FLAGS)

hTable.ow: hTable.c
	x86_64-w64-mingw32-gcc-win32 hTable.c -o hTable.ow -c $(FLAGS)

model.ow: model.c
	x86_64-w64-mingw32-gcc-win32 model.c -o model.ow -c $(FLAGS)

regionFileReader.exe: regionParser.ow regionFileReader.c
	x86_64-w64-mingw32-gcc-win32 regionFileReader.c regionParser.ow -o regionFileReader.exe -lz -static $(FLAGS)

chunkExtractor.exe: regionParser.ow chunkExtractor.c
	x86_64-w64-mingw32-gcc-win32 chunkExtractor.c regionParser.ow -o chunkExtractor.exe -lz -lm -static $(FLAGS)

generator.ow: generator.c
	x86_64-w64-mingw32-gcc-win32 generator.c -o generator.ow -c $(FLAGS)

modelGenerator.exe: generator.ow model.ow chunkParser.ow hTable.ow cNBT.ow modelGenerator.c
	x86_64-w64-mingw32-gcc-win32 modelGenerator.c generator.ow model.ow chunkParser.ow hTable.ow cNBT.ow -o modelGenerator.exe -lm $(FLAGS)

windows: modelGenerator.exe chunkExtractor.exe regionFileReader.exe

clean:
	rm -f *.o
	rm -f cNBT/*.o
	rm -f *.ow
	rm -f cNBT/*.ow

check: hTable.o regionParser.o
	#hTable tests
	checkmk tests/hTable.check > tests/hTableCheck.c
	gcc tests/hTableCheck.c hTable.o -lcheck -lm -lsubunit -Wall -o tests/hTableCheck
	./tests/hTableCheck
	#regionParser tests
	checkmk tests/regionParser.check > tests/regionParserCheck.c
	gcc tests/regionParserCheck.c regionParser.o -lcheck -lm -lz -lsubunit -o tests/regionParserCheck
	./tests/regionParserCheck
	#unit tests run, now just run the programs
	./chunkExtractor ./tests 0 0
	./modelGenerator ./tests/0.0.nbt