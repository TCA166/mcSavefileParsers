
hTableCheck.c: hTable.check
	checkmk hTable.check > hTableCheck.c

hTableCheck: hTableCheck.c ../hTable.o
	gcc hTableCheck.c ../hTable.o -lcheck -lm -lsubunit -o hTableCheck

regionParserCheck.c: regionParser.check
	checkmk regionParser.check > regionParserCheck.c

regionParserCheck: regionParserCheck.c ../regionParser.o
	gcc regionParserCheck.c ../regionParser.o -lcheck -lm -lz -lsubunit -o regionParserCheck

chunkParserCheck.c: chunkParser.check
	checkmk chunkParser.check > chunkParserCheck.c

chunkParserCheck: chunkParserCheck.c ../chunkParser.o ../cNBT.o
	gcc chunkParserCheck.c ../chunkParser.o ../cNBT.o -lcheck -lm -lsubunit -o chunkParserCheck

modelCheck.c: model.check
	checkmk model.check > modelCheck.c

modelCheck: modelCheck.c ../model.o ../hTable.o
	gcc modelCheck.c ../model.o ../hTable.o -lcheck -lm -lsubunit -o modelCheck -g

clean:
	rm -f *.c
