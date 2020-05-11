CC=gcc
CFLAGS=-I. -std=c99

all: parallel_sum


parallel_sum: utils.o parallel_sum.c
	$(CC) -o parallel_sum parallel_sum.c utils.o -lpthread $(CFLAGS)

utils.o: utils.h utils.c
	$(CC) -o utils.o -c utils.c $(CFLAGS)

clean: rm -f utils.o parallel_sum parallel_sum.o
