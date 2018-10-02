#ifndef __LinearHash__
#define	 __LinearHash__

#include "trie.h"

#define HASHSTARTINGSIZE_ 	2000
#define BUCKETSIZE 			4

typedef struct trie_node trie_node;

typedef struct Bucket{

	int 		maxsize;
	int 		currentsize;
	trie_node* 	bucketnodes;

}Bucket;

typedef struct HashInfo{

	int 		maxsize;		// real size
	int 		currentroundsize;	// size at the start of current round
	int 		pointer;
	int 		round;
	int 		sumofElements;
	int 		overflows;
	Bucket* 	buckets;

}HashInfo;


HashInfo*	create_Hash();
//STATIC
trie_node*  static_lookup_Hash(HashInfo* myHash, char* word);
// Regular
trie_node*  lookup_Hash(HashInfo* myHash, char* word);
trie_node* 	add_HashInfo(HashInfo* myHash, char* word, int final, int version);

int   		add_bucket(Bucket* bucket, trie_node* add, int pos);
int 		deleteFromHash(HashInfo* myHash, char* word, int final);
int 		reHashNode(HashInfo* myHash, int bucketpos, int i);
int 		hash_Function(HashInfo* myHash, char* word);

void		split_bucket(HashInfo* myHash);
void 		enlarge_bucket(Bucket* bucket);
void 		delete_HashInfo(HashInfo* myHash);
void 		print_myHash(HashInfo* myHash);

#endif