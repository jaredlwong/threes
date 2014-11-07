.PHONY: threes test

threes:
	gcc-4.8 -std=c99 threes.c main.c -o threes

test:
	gcc-4.8 -std=c99 -gdwarf-2 -g3 threes.c test.c -o test
