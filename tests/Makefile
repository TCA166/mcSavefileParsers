
hTableCheck.c: hTable.check
	checkmk hTable.check > hTableCheck.c

hTableCheck: hTableCheck.c ../hTable.o
	gcc hTableCheck.c ../hTable.o -lcheck -lm -lsubunit -o hTableCheck

regionParserCheck.c: regionParser.check
	checkmk regionParser.check > regionParserCheck.c

regionParserCheck: regionParserCheck.c ../regionParser.o
	gcc regionParserCheck.c ../regionParser.o -lcheck -lm -lsubunit -o regionParserCheck

clean:
	rm -f *.c
