#ifndef __HASH_
#define __HASH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Datastruct.h"

#define BUCKETSIZ 	5 
#define 	SUM 	"sum"
#define 	OUT		"out"
#define 	IN 		"in"
#define DUMP		"dump"
#define CRE_NOD 	"createnodes"
#define DEL_NOD		"delnodes"
#define ADD_TR		"addtran"
#define DEL_TR		"deltran"
#define LOOKUP 		"lookup"
#define TRIAN		"triangle"
#define CONN		"conn"
#define ALLCYCLES	"allcycles"
#define TRACE		"traceflow"
#define BYE			"bye"
#define PRINT 		"print"
typedef struct Bucket{
	struct Bucket   * 	next;
	graphNode*			komvoi[BUCKETSIZ];
}Bucket;


typedef struct Hashinfo{
	int size;
	int elements;
	Bucket*	table;
}Hashinfo;


Hashinfo*	 hashCreation(int hashsize);					// creates hash
int 		keyfunction(Hashinfo* hash,graphNode* target);	// uses the key function on the Node and returns it's position in hash
int 		addElement(Bucket* target,graphNode* data);		// places the node on the bucket 
int 		addBucket(Bucket* target,graphNode* data);		// adds bucket with an element if other bucket is full
int 		insertHash(Hashinfo* hash,graphNode* element);	// inserts element in hash using the above fuctions
graphNode*	removefromhash(Hashinfo* hash,int id);			// removes the element with key=id
graphNode* 	searchHash(Hashinfo* hash,int id);				// searchs hash with key=id
int 		runthroughHash(Hashinfo* hash);					// run throughs whole hash and resets "check-marking"

void 	printHash(Hashinfo* hash);							// prints hashtable
void 	resetHash(Hashinfo* hash);							// erase all data of hash but keeps it empty
void	dump(Hashinfo* hashtable,char* filename);			// puts all data of hash in file "filename"


int 	createNode(Hashinfo* hashtable, int id);			//creates a node with name=id and puts in Hash
int 	deleteNode(Hashinfo* hashtable, int id);			//deletes a node with name=id and removes from Hash
int 	addtran(Hashinfo* hashtable, int source, int target, double amount);	//adds/updates an edge between 2 nodes 
int 	remtran(Hashinfo* hashtable, int source, int target);					//removes an edge between 2 nodes 
int 	triangle(Hashinfo* hashtable,int start,double minimum);					//prints all circles size of 3 which have the node with id=start in them 
int 	connection(Hashinfo* hashtable,int start,int end);					//finds a path between 2 nodes if it exists
int 	allcycle(Hashinfo*hashtable,int start);								//finds all circles involving node with id=start
int 	traceflow(Hashinfo*hashtable,int start,int length);					//finds all paths size of "length" involving node with id=start

double 	look_up(Hashinfo* hashtable,char* command, int target,int* error);	//prints info for the nodes depending the "command"=sum|in|out



#endif