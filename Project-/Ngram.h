
#ifndef __Ngram__
#define	 __Ngram__

#include "header.h"
#define STARTINGSIZE 5000
#define WORDLIMIT  50

typedef struct Ngram{
	char(* wordBuffer)[WORDLIMIT]; 
	int currentsize;
	int maxCapacity;

}Ngram;	

int 	createNgram(Ngram** myNgram);
int 	enlargeNgram(Ngram *myNgram);

int 	addinNgram(Ngram* myNgram,char* word);

char* 	getWordFromNgram(Ngram* myNgram, int pos);

void 	printNgram(Ngram* myNgram);
void 	resetNgram(Ngram* myNgram);
void 	deleteNgram(Ngram** myNgram);

#endif