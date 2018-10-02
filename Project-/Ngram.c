#include "Ngram.h"
/*
char (*orderedIds)[ID_LEN+1]
    = malloc(variableNumberOfElements * sizeof(*orderedIds));

// Clear-up
free(orderedIds);
*/

// Ngram struct Initialization
int 	createNgram(Ngram** myNgram){
	*myNgram = malloc(sizeof(Ngram));
	if(myNgram==NULL){
		printf("memory error\n");
		return MEMERR;
	}
	
	(*myNgram)->wordBuffer = malloc( STARTINGSIZE*WORDLIMIT);
	if((*myNgram)->wordBuffer == NULL){
		printf("memory error\n");
		return MEMERR;
	}
	//*(*myNgram)->wordBuffer = buf;
	(*myNgram)->currentsize = 0;
	(*myNgram)->maxCapacity = STARTINGSIZE;
	return 0;
}

// doubles the Ngram string array
int 	enlargeNgram(Ngram *myNgram){

	if(myNgram->currentsize < myNgram->maxCapacity){
	//	printf("not full capacity\n");
		return BADUSE;
	}
	
	int newMax = 2* myNgram->maxCapacity;
	myNgram->wordBuffer = realloc(myNgram->wordBuffer,WORDLIMIT*newMax);
	
	if(myNgram->wordBuffer == NULL){
		printf("realloc failed\n");
		return MEMERR;
	}
	myNgram->maxCapacity = newMax;

	return 0;
}
// adds word in the last available position in Ngram array
int 	addinNgram(Ngram* myNgram,char* word){
	if(myNgram->currentsize == myNgram->maxCapacity){
		enlargeNgram(myNgram);
	}

	strcpy(myNgram->wordBuffer[myNgram->currentsize], word);
	myNgram->currentsize++;
	return 0;
}

// get a word from position "pos" in the Ngram array
char* 	getWordFromNgram(Ngram* myNgram, int pos){
	if(myNgram->currentsize <= pos){
		return NULL;
	}
	return myNgram->wordBuffer[pos];
}

// print all the word in Ngram
void 	printNgram(Ngram* myNgram){
	int i;
	for(i=0; i < myNgram->currentsize; i++){
		printf(" %d word is : %s\n",i, myNgram->wordBuffer[i]);
	}
}

// resets Ngram for reuse 
void 	resetNgram(Ngram* myNgram){
	myNgram->currentsize = 0;
	return;
}

// deallocates Ngram memory
void 	deleteNgram(Ngram** myNgram){
	free((*myNgram)->wordBuffer);
//	printf("hi\n");
	free(*myNgram);
}