CC=gcc
CFLAGS=-I. -Wall

all : sequential_min_max parallel_min_max sep_proc_min_max

sep_proc_min_max : sep_proc_min_max.o
	$(CC) $(CFLAGS) -o sep_proc_min_max sep_proc_min_max.o

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) $(CFLAGS) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c 

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) $(CFLAGS) -o utils.o -c utils.c 

find_min_max.o : utils.h find_min_max.h
	$(CC) $(CFLAGS) -o find_min_max.o -c find_min_max.c 

sep_proc_min_max.o : 
	$(CC) $(CFLAGS) -o sep_proc_min_max.o -c sep_proc_min_max.c

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
