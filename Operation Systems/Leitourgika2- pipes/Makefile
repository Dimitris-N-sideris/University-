CC= gcc
CFLAGS= -g -Wall
all:sorter merger tally

merger: splitmerger.o DoubleL_pointer.o Datastruct.o
	gcc -o merger splitmerger.o  DoubleL_pointer.o Datastruct.o

splitmerger.o: splitmerger.c DoubleL_pointer.c Datastruct.c
	$(CC) $(CFLAGS) -c splitmerger.c

DoubleL_pointer.o:   DoubleL_pointer.c Pointer_List.h Datastruct.c record.h
	$(CC) $(CFLAGS) -c DoubleL_pointer.c 

Datastruct.o:	Datastruct.c record.h
	$(CC) $(CFLAGS) -c Datastruct.c


sorter: sorter.o  DoubleL_pointer.o Datastruct.o LinearHash.o
	gcc -o sorter sorter.o DoubleL_pointer.o LinearHash.o Datastruct.o

sorter.o:   sorter.c DoubleL_pointer.c Datastruct.c LinearHash.c LinearHash.h Pointer_List.h record.h
	$(CC) $(CFLAGS) -c sorter.c

LinearHash.o: DoubleL_pointer.c Pointer_List.h Datastruct.c Datastruct.h LinearHash.c LinearHash.h
	$(CC) $(CFLAGS) -c LinearHash.c

tally: root.o DoubleL_pointer.o Datastruct.o
	gcc -o tally root.o	DoubleL_pointer.o Datastruct.o

root.o:	root.c
	$(CC) $(CFLAGS) -c root.c
clean: 
	$(RM) twa *.o *~
