
SUBUNIT := -lsubunit

ZLIB := -lz

#we want to check what enviroment are we compiling under
UNAME := $(shell uname)
ifneq (,$(findstring CYGWIN,$(UNAME))) 
    ZLIB = -Wl,-Bstatic -lz -Wl,-Bdynamic
endif
ifneq (,$(findstring MSYS,$(UNAME))) 
    ZLIB = -Wl,-Bstatic -lz -Wl,-Bdynamic
	SUBUNIT = 
endif

all: CFLAGS := -O3
all: radiusGenerator modelGenerator chunkExtractor regionFileReader

debug: CFLAGS := -Wall -Werror -Wpedantic -g
debug: radiusGenerator modelGenerator chunkExtractor regionFileReader

cNBT.o: 
	gcc cNBT/buffer.c -o cNBT/buffer.o -c $(CFLAGS)
	gcc cNBT/nbt_parsing.c -o cNBT/nbt_parsing.o -c $(CFLAGS)
	gcc cNBT/nbt_treeops.c -o cNBT/nbt_treeops.o -c $(CFLAGS)
	gcc cNBT/nbt_util.c -o cNBT/nbt_util.o -c $(CFLAGS)
	ld -relocatable cNBT/buffer.o cNBT/nbt_parsing.o cNBT/nbt_treeops.o cNBT/nbt_util.o -o cNBT.o

regionParser.o: src/lib/regionParser.c
	gcc src/lib/regionParser.c -o regionParser.o -c -lm $(CFLAGS)

chunkParser.o: src/lib/chunkParser.c
	gcc src/lib/chunkParser.c -o chunkParser.o -c -lm $(CFLAGS)

hTable.o: src/lib/hTable.c
	gcc src/lib/hTable.c -o hTable.o -c $(CFLAGS)

model.o: src/lib/model.c
	gcc src/lib/model.c -o model.o -c $(CFLAGS)

regionFileReader: regionParser.o src/regionFileReader.c
	gcc src/regionFileReader.c regionParser.o -o regionFileReader $(ZLIB) -lm $(CFLAGS)

chunkExtractor: regionParser.o src/chunkExtractor.c
	gcc src/chunkExtractor.c regionParser.o -o chunkExtractor $(ZLIB) -lm $(CFLAGS)

generator.o: src/lib/generator.c
	gcc src/lib/generator.c -o generator.o -c $(CFLAGS)

modelGenerator: model.o generator.o hTable.o chunkParser.o cNBT.o src/modelGenerator.c
	gcc src/modelGenerator.c generator.o model.o hTable.o chunkParser.o cNBT.o -o modelGenerator -lm $(CFLAGS)

radiusGenerator: model.o generator.o hTable.o chunkParser.o regionParser.o cNBT.o src/radiusGenerator.c
	gcc src/radiusGenerator.c generator.o model.o regionParser.o hTable.o chunkParser.o cNBT.o $(ZLIB) -lm -o radiusGenerator $(CFLAGS)

cNBT.ow:
	x86_64-w64-mingw32-gcc-win32 cNBT/buffer.c -o cNBT/buffer.ow -c $(CFLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_parsing.c -o cNBT/nbt_parsing.ow -c $(CFLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_treeops.c -o cNBT/nbt_treeops.ow -c $(CFLAGS)
	x86_64-w64-mingw32-gcc-win32 cNBT/nbt_util.c -o cNBT/nbt_util.ow -c $(CFLAGS)
	x86_64-w64-mingw32-ld -relocatable cNBT/buffer.ow cNBT/nbt_parsing.ow cNBT/nbt_treeops.ow cNBT/nbt_util.ow -o cNBT.ow

regionParser.ow: src/lib/regionParser.c
	x86_64-w64-mingw32-gcc-win32 src/lib/regionParser.c -o regionParser.ow -c $(CFLAGS)

chunkParser.ow: src/lib/chunkParser.c
	x86_64-w64-mingw32-gcc-win32 src/lib/chunkParser.c -o chunkParser.ow -c -lm $(CFLAGS)

hTable.ow: src/lib/hTable.c
	x86_64-w64-mingw32-gcc-win32 src/lib/hTable.c -o hTable.ow -c $(CFLAGS)

model.ow: src/lib/model.c
	x86_64-w64-mingw32-gcc-win32 src/lib/model.c -o model.ow -c $(CFLAGS)

regionFileReader.exe: regionParser.ow src/regionFileReader.c
	x86_64-w64-mingw32-gcc-win32 src/regionFileReader.c regionParser.ow -o regionFileReader.exe -lz -static $(CFLAGS)

chunkExtractor.exe: regionParser.ow src/chunkExtractor.c
	x86_64-w64-mingw32-gcc-win32 src/chunkExtractor.c regionParser.ow -o chunkExtractor.exe -lz -static $(CFLAGS)

generator.ow: src/lib/generator.c
	x86_64-w64-mingw32-gcc-win32 src/lib/generator.c -o generator.ow -c $(CFLAGS)

modelGenerator.exe: generator.ow model.ow chunkParser.ow hTable.ow cNBT.ow src/modelGenerator.c
	x86_64-w64-mingw32-gcc-win32 src/modelGenerator.c generator.ow model.ow chunkParser.ow hTable.ow cNBT.ow -o modelGenerator.exe -lm $(CFLAGS)

radiusGenerator.exe: model.ow generator.ow hTable.ow chunkParser.ow regionParser.ow cNBT.ow src/radiusGenerator.c
	x86_64-w64-mingw32-gcc-win32 src/radiusGenerator.c generator.ow model.ow regionParser.ow hTable.ow chunkParser.ow cNBT.ow -lm -static -lz -o radiusGenerator.exe $(CFLAGS)

windows: modelGenerator.exe chunkExtractor.exe regionFileReader.exe radiusGenerator.exe

clean:
	rm -f *.o
	rm -f cNBT/*.o
	rm -f *.ow
	rm -f cNBT/*.ow
	rm -f radiusGenerator
	rm -f radiusGenerator.exe
	rm -f modelGenerator
	rm -f modelGenerator.exe
	rm -f chunkExtractor
	rm -f chunkExtractor.exe
	rm -f regionFileReader
	rm -f regionFileReader.exe

check: hTable.o regionParser.o chunkParser.o cNBT.o model.o
	#hTable tests
	checkmk tests/hTable.check > tests/hTableCheck.c
	gcc tests/hTableCheck.c hTable.o -lcheck -lm $(SUBUNIT) -Wall -o tests/hTableCheck
	./tests/hTableCheck
	#regionParser tests
	checkmk tests/regionParser.check > tests/regionParserCheck.c
	gcc tests/regionParserCheck.c regionParser.o -lcheck -lm $(ZLIB) $(SUBUNIT) -o tests/regionParserCheck
	./tests/regionParserCheck
	#chunkParser tests
	checkmk tests/chunkParser.check > tests/chunkParserCheck.c
	gcc tests/chunkParserCheck.c chunkParser.o cNBT.o -lcheck -lm $(SUBUNIT) -o tests/chunkParserCheck
	./tests/chunkParserCheck
	#model tests
	checkmk tests/model.check > tests/modelCheck.c
	gcc tests/modelCheck.c model.o hTable.o -lcheck -lm $(SUBUNIT) -o tests/modelCheck
	./tests/modelCheck