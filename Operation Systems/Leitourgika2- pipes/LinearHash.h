#ifndef LINEARHASH_H
#define LINEARHASH_H

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<malloc.h>

#include 	"record.h"

#define 	ARXIKO 5



typedef struct Bucket_element{
	char					key[24];
	tally 					data;
}Element;



typedef struct HashBucket{
	
	Element *			bucket; 			// dunamika analoga me to BucketSize
	struct HashBucket *	next;				// to epomeno Bucket 
	
}Bucket;

typedef struct Infobucket{
	int 				number_of_elements;	// posa bucket uparxoun edw genika
	Bucket *			arxikos;
}InfoBucket;


typedef struct infohash{
	InfoBucket* table;
	int			nextForSplit;		
	int 		round;
	int 		size;
	int			elements;
	int 		bucketSize;
	double 		loadFactor;
}infohash;

tally*			getMax(InfoBucket* Info, int Bucket_Size, tally* max);
tally*			check(infohash *hash,char* kwdikos);
void 			printsorted(infohash* hash,int output);
void 			printInfoHash(infohash* hash,int output);
void 			printBucket(InfoBucket* Info, int Bucket_Size,int output);	
void 			split(infohash* hash);
int 			increaseRound(infohash** hash);
void 			katastrofhHashtable(infohash ** hash);
void 			katastrofhBucket(Bucket * temp);
void 			setElement(Element* target,char* kleidi,tally* temp);
int 			insert_hash(infohash* hash,tally* input);

tally* 			getElement(Bucket * target, int pos);

int 			clean_bucket(Bucket * temp,int bucketsize);
int 			getKey(infohash* hash, char* kwdikos,int mode);
int 			checkForSplit(infohash * hash);
int 			initializeBucket(Bucket * target,int num_buckets);
int 			newBucket(InfoBucket * target,int num_buckets);
int 			Hash_creation(infohash** hash,int bucket_Size,float load_Factor);
int 			putInBucket(int bucketsize,InfoBucket* target,tally* input);
//int 			add_hash(infohash* hash,tally* input);
#endif