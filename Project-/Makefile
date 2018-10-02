CC= gcc
CFLAGS= -g -Wall
LFLAGS= -lpthread

#compilation files

all:    trie Ngram LinearHash murmur3 main TopK JobScheduler

Ngram: Ngram.c Ngram.h
	gcc -g -Wall -O2 -c Ngram.c 

LinearHash: LinearHash.c LinearHash.h
	gcc -g -Wall -O2 -c LinearHash.c

trie: trie.c trie.h
	gcc -g -Wall -O2 -c trie.c

JobScheduler: JobScheduler.c JobScheduler.h
	gcc -g -Wall -O2 -c -pthread JobScheduler.c

TopK: TopK.c TopK.h
	gcc -g -Wall -O2 -c TopK.c

murmur3:murmur3.c murmur3.h
	gcc -g -Wall -O2 -c murmur3.c

main: main.c header.h  trie.o Ngram.o LinearHash.o TopK.o JobScheduler.o 
	gcc -g -Wall -O2 -pthread  main.c Ngram.o trie.o murmur3.o  TopK.o LinearHash.o JobScheduler.o -o  myexe 

test:  mockmain.c header.h  trie.o Ngram.o LinearHash.o TopK.o
	gcc -g -Wall   mockmain.c Ngram.o trie.o murmur3.o TopK.o  LinearHash.o -o  mytest 


#RUN SAMPLES
exe:
	./myexe -i test.init -q test.work

unit:
	./mytest

dyna:
	./myexe -i small_dynamic.init -q small_dynamic.work

stat:
	./myexe -i small_static.init -q small_static.work

med:
	./myexe -i medium_dynamic.init -q medium_dynamic.work

med1:
	./myexe -i medium_static.init -q medium_static.work

s1:
	./myexe -i small.init -q small.work

val:
	valgrind  --leak-check=full --show-leak-kinds=all ./myexe -i test.init -q test.work

clean: 
	$(RM) myexe *.o *~



