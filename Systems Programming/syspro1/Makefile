CC= gcc
CFLAGS= -g -Wall
all:ergasia

ergasia: Hash.o Datastruct.o main.o 
	gcc -o ergasia main.o  Datastruct.o Hash.o

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c

Hash.o:   Hash.c Hash.h Datastruct.c Datastruct.h
	$(CC) $(CFLAGS) -c Hash.c 

Datastruct.o:	Datastruct.c Datastruct.h
	$(CC) $(CFLAGS) -c Datastruct.c

clean: 
	$(RM) ergasia *.o *~
