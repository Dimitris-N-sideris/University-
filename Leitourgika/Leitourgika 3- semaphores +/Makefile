CC= gcc
CFLAGS= -g -Wall
LFLAGS= -lpthread
all:orchestrator counter checker voter

orchestrator: orchestrator.c Datastruct.h
	gcc orchestrator.c -o  orchestrator -lpthread
counter: counter.c Datastruct.h
	gcc counter.c -o  counter -lpthread
checker: checker.c Datastruct.h
	gcc checker.c -o  checker -lpthread
voter: voter.c Datastruct.h
	gcc voter.c -o  voter -lpthread

clean: 
	$(RM) twa *.o *~



