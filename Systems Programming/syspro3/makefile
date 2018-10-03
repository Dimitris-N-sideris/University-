CC= gcc
CFLAGS= -g -Wall
all: server client

server: bankserver.o domh.o
	gcc -o server -pthread bankserver.o domh.o 

bankserver.o: bankserver.c 
	$(CC) $(CFLAGS) -c bankserver.c 

domh.o: domh.c
	$(CC) $(CFLAGS) -c domh.c 

client:  client.o
	gcc -o client  client.o domh.o 

client.o:	client.c
	$(CC) $(CFLAGS) -c  client.c 


clean: 
	$(RM) server client *.o *~
