
hTableCheck.c: hTable.check
	checkmk hTable.check > hTableCheck.c

hTableCheck: hTableCheck.c ../hTable.o
	gcc hTableCheck.c ../hTable.o -lcheck -lm -lsubunit -o hTableCheck

clean:
	rm -f *.c
