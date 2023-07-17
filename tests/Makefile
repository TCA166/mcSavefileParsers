
hTableCheck.c: hTable.check
	checkmk hTable.check > hTableCheck.c

hTableCheck: hTableCheck.c
	gcc hTableCheck.c ../hTable.o -lcheck -lm -lsubunit -Wall -o hTableCheck
	./hTableCheck
	rm hTableCheck