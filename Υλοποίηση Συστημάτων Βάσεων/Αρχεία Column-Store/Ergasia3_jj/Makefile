CC= gcc
CFLAGS= -g -Wall
all: ergasia
ergasia: main.o  column_store.o 
	gcc -o ergasia main.o column_store.o  BF_32.a 

main1.o: main1.c column_store.c column_store.h
	$(CC) $(CFLAGS) -c main.c

column_store.o: column_store.c column_store.h BF_32.a
	$(CC) $(CFLAGS) -c column_store.c
