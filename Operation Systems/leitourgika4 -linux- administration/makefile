CC= gcc
CFLAGS= -g -Wall
all:main

main: leitourgies.o main.o list.o
	$(CC) $(CFLAGS) -o ergasia leitourgies.o list.o main.o 
main.o:	leitourgies.c main.c leitourgies.h
	$(CC) $(CFLAGS) -c main.c
leitourgies.o: leitourgies.c leitourgies.h list.h list.c
	$(CC) $(CFLAGS) -c leitourgies.c 
list.o: list.c 	list.h
	$(CC) $(CFLAGS) -c list.c 