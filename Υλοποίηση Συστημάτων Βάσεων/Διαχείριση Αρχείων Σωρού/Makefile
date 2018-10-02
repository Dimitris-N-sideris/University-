CC= gcc
CFLAGS= -g -Wall
all: ergasia
ergasia: main.o  heap.o 
	gcc -o ergasia main.o heap.o  BF_32.a 

main.o:   main.c heap.c heap.h
	$(CC) $(CFLAGS) -c main.c

heap.o: 	heap.h BF_32.a
	$(CC) $(CFLAGS) -c heap.c

