CC= gcc
CFLAGS= -g -Wall
all: ergasia
ergasia: main.o Btree.o AM.o 
	gcc -o ergasia main.o AM.o Btree.o BF_32.a 

main.o: main.c AM.c AM.h Btree.c Btree.h
	$(CC) $(CFLAGS) -c main.c

Btree.o:Btree.c Btree.h defn.h BF_32.a
	$(CC) $(CFLAGS) -c Btree.c

AM.o: AM.c AM.h Btree.c Btree.h defn.h BF_32.a
	$(CC) $(CFLAGS) -c AM.c
