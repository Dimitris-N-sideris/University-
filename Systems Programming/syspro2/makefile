CC= gcc
CFLAGS= -g -Wall
all: board boardpost

board: queue.o board.o
	gcc -o board queue.o board.o 

queue.o: queue.c 
	$(CC) $(CFLAGS) -c queue.c 

board.o: board.c queue.c
	$(CC) $(CFLAGS) -c board.c



boardpost:  boardpost.o
	gcc -o boardpost  boardpost.o queue.o

boardpost.o:	boardpost.c
	$(CC) $(CFLAGS) -c  boardpost.c 


clean: 
	$(RM) boardpost board *.o *~
