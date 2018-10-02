#include	"LinearHash.h"


HashInfo* create_Hash(){

	HashInfo* created = malloc(sizeof(HashInfo));
	if(created == NULL){
		printf("Not enough memory for HashInfo\n");
		exit(MEMERR);
	}
	created->buckets = malloc(sizeof(Bucket)*HASHSTARTINGSIZE_);
	if(created->buckets == NULL){
		printf("Not enough memory for buckets\n");
		exit(MEMERR);
	}	
	created->maxsize 			= HASHSTARTINGSIZE_;	// realsize
	created->currentroundsize 	= HASHSTARTINGSIZE_;	// round start size
	created->round 				= 0;
	created->pointer 			= 0;
	created->sumofElements 		= 0;
	created->overflows 			= 0;
	memset(created->buckets, 0, sizeof(Bucket)*HASHSTARTINGSIZE_);

	return created;
}

int uninitialized_bucket(Bucket* bucket){
	if(bucket->maxsize == 0){
		return 1;
	}
	return 0;
}

int  hash_Function(HashInfo* myHash, char* word){

	/* hash function taken from stack overflow */
	unsigned long hash = 5381;
    int c;

    while ((c = *word++)!= 0)
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    /* hash function taken from stack overflow */
   	
   	int key = hash % myHash->currentroundsize;

   	if(key < myHash->pointer){

   		key = hash % (myHash->currentroundsize*2);
  	}

   return key;
}

trie_node*  lookup_Hash(HashInfo* myHash, char* word){

	if(myHash == NULL || word == NULL){
	
		printf("Null pointer\n");
		return NULL;
	}

	int key = hash_Function(myHash, word);
	int ordered_position = 0;
	Bucket* targetBucket = &(myHash->buckets[key]);

	if(uninitialized_bucket(targetBucket) == 1){
		//printf("emty buck: %s\n",word);
		return NULL;
	}

	int found = binary_search(targetBucket->bucketnodes, word, targetBucket->currentsize, &ordered_position);
	if(found < 0){ // add
	//	printf("not found %s\n",word);
		return NULL;
	}
	else{
		return &(targetBucket->bucketnodes[found]);
	}
}


trie_node*  static_lookup_Hash(HashInfo* myHash, char* word){

	if(myHash == NULL || word == NULL){
	
		printf("Null pointer\n");
		return NULL;
	}
	
	int key = hash_Function(myHash, word);
	int ordered_position = 0;
	Bucket* targetBucket = &(myHash->buckets[key]);

	if(uninitialized_bucket(targetBucket) == 1){
		//printf("emty buck: %s\n",word);
		return NULL;
	}

	int found = static_binary_search(targetBucket->bucketnodes, word, targetBucket->currentsize, &ordered_position);
	if(found < 0){ // add
	//	printf("not found %s\n",word);
		return NULL;
	}
	else{
		return &(targetBucket->bucketnodes[found]);
	}
}

trie_node* 	add_HashInfo(HashInfo* myHash, char* word, int final, int version){
	
	if(myHash == NULL || word == NULL){
	
		printf("Null pointer\n");
		return NULL;
	}
	
	int key = hash_Function(myHash, word);
	
	int ordered_position = 0;
	Bucket* targetBucket = &(myHash->buckets[key]);
	if(uninitialized_bucket(targetBucket) == 1){
		targetBucket->bucketnodes = malloc(sizeof(trie_node)*BUCKETSIZE);
		targetBucket->maxsize = BUCKETSIZE;
	}
	int found = binary_search(targetBucket->bucketnodes, word, targetBucket->currentsize, &ordered_position);
	if(found < 0){ // add

		trie_node adding;
		memset(&adding, 0, sizeof(trie_node));
	//	adding.word = NULL;
		copy_word(&adding, word);
		adding.final = final;
		if(final == TRUE){
			adding.add_version = version;
		}
		myHash->sumofElements++;
		if(add_bucket(targetBucket, &adding, ordered_position) == 0){
			
			return &(targetBucket->bucketnodes[ordered_position]);
		}
		else{	// split + lookup -> return
			
			split_bucket(myHash);
			// print_myHash(myHash);
			return lookup_Hash(myHash, word);
		}
		
	}
	else{
		if(targetBucket->bucketnodes[found].final == FALSE){
			if(final == TRUE){
				targetBucket->bucketnodes[found].add_version = version;
				targetBucket->bucketnodes[found].final = final;
			}
			
		}
		return &(targetBucket->bucketnodes[found]);
	}
}


void		split_bucket(HashInfo* myHash){

	myHash->overflows++;
	if(myHash->maxsize == myHash->currentroundsize*2){

		myHash->currentroundsize = myHash->maxsize;
		myHash->round++;
		myHash->pointer = 0;
	}

	myHash->maxsize++;
	
	Bucket* enlarged = realloc(myHash->buckets,sizeof(Bucket)*myHash->maxsize);
	if(enlarged == NULL){
		printf("Couldn't split\n");
		exit(MEMERR);
	}

	enlarged[myHash->maxsize - 1].maxsize 		= 0;
	enlarged[myHash->maxsize - 1].currentsize 	= 0;
	enlarged[myHash->maxsize - 1].bucketnodes	= NULL;
	myHash->buckets = enlarged;
	
	int i, pos = myHash->pointer;
	Bucket* 	change 		= &myHash->buckets[pos];
	myHash->pointer++;
	for( i = 0; i < change->currentsize; i++){
		
		if (reHashNode(myHash, pos, i) == 1)	i--;
	}	
	
}

int reHashNode(HashInfo* myHash, int bucketpos, int i){

	Bucket* 	change 		= &myHash->buckets[bucketpos];
	trie_node*	checking 	= &(change->bucketnodes[i]);
	char 	word[WORDLIMIT];
	if(checking->wordsizetable == NULL) {
		strcpy(word, checking->word);
	}
	else{
		printf("DANGER\n\n\n");
		int i, wordlength = checking->wordsizetable[0];
	
		if(wordlength < 0) 					wordlength *= -1;
	
		for(i = 0; i < wordlength; i++) 	word[i] = checking->word[i];
	
		word[wordlength + 1] = '\0';
	}
	int key = hash_Function(myHash, word);
	
	if(key != bucketpos){		// node position must be changed to the new bucket

		Bucket* newplace = &(myHash->buckets[key]);
		int ordered_position = 0;
		binary_search(newplace->bucketnodes, checking->word, newplace->currentsize, &ordered_position);
		add_bucket(newplace, checking, ordered_position);
		change->currentsize--;
		int move = (change->currentsize - i) * sizeof(trie_node);		// how many bytes to shift with memmove												

		if (move > 0) {	// if deleted element is not last

			memmove(&(change->bucketnodes[i]), &(change->bucketnodes[i + 1]), move);
		}
		
		node_reset(&change->bucketnodes[change->currentsize]); // reset last element
		return 1;
	}

	return 0;
}


int   	add_bucket(Bucket* bucket, trie_node* add, int pos){
	if(bucket == NULL || add == NULL){
		printf("addbucket null pointer\n");
		exit(BADUSE);
	}
	int return_val = 0;
	if(uninitialized_bucket(bucket) == 1){

		bucket->bucketnodes = malloc(sizeof(trie_node)*BUCKETSIZE);
		bucket->maxsize = BUCKETSIZE;
	}
	if(bucket->currentsize == bucket->maxsize){// overflow -> enlarge bucket

		//printf("overflow\n");
		enlarge_bucket(bucket);	
		return_val = 1;
	}

	// add it 
	if (bucket->currentsize != pos && bucket->currentsize != 0) {

		int move = (bucket->currentsize - pos)*sizeof(trie_node);
		memmove(&bucket->bucketnodes[pos + 1], &bucket->bucketnodes[pos], move);	
	}
	bucket->currentsize++;

	// gia thn periptwhsh pou meta apo ena overflow meiwthoun ta stoixeia tou bucket kai 3anaperasoun ta oria (apofugei) perittwn realloc
	if( (bucket->currentsize%BUCKETSIZE != 0 ) && ((bucket->currentsize-1)%BUCKETSIZE == 0) && (bucket->currentsize > BUCKETSIZE)){ 
		// To stoixeio pou prostethike dn tha xwrouse xwris na ginei overflow sto  bucket 
		//printf("fake overflow\n");
		return_val = 1; 
	}
	memcpy(&bucket->bucketnodes[pos], add, sizeof(trie_node));	
	return return_val;
}

int 		deleteFromHash(HashInfo* myHash, char* word, int rootngram){

	int key = hash_Function(myHash, word);
	Bucket* target = &(myHash->buckets[key]);
	int ordered_position = 0;

	int found = binary_search(target->bucketnodes, word, target->currentsize, &ordered_position);
	if(found < -1){
		return -1;
	}
	trie_node* to_delete = &(target->bucketnodes[found]);
	if(rootngram == 1){// if ngram was 1 word length then 
		/*if(to_delete->final == 0){ //this word should have been final to be deleted
			return -1;
		}
		if(to_delete->added > 0){	// it is ngram but has childrens 
			to_delete->final = 0;	// just unmark it
			return -1;
		}*/
	}
	else{	// part of ngram larger than size of 1 word
		if(to_delete->added > 0 || to_delete->final == 1){	// has children / is ngram itself
			return -1;										// dt delete
		}
	}
	/*//printf("%s...%s.\n",word, to_delete->word);
	delete_node(to_delete);
	target->bucketnodes[found].word = NULL;
	target->bucketnodes[found].word_spaceleft = 0 ;
	target->currentsize--;
	int move = (target->currentsize - found) * sizeof(trie_node);		// how many bytes to shift with memmove												
	if (move > 0) {	// if deleted element is not last

		memmove(&(target->bucketnodes[found]), &(target->bucketnodes[found + 1]), move);
	}
//	printf("\ni7 LH\n");
	node_reset(&target->bucketnodes[target->currentsize]); // reset last element
	*/	

	return 0;
}

void print_myHash(HashInfo* myHash){
	int i,j;
	for(i = 0; i< myHash->maxsize; i++){
		Bucket * current= &(myHash->buckets[i]);
		printf("bucket %d:",i);
		for (j = 0; j < current->currentsize; ++j)
		{
			if(hash_Function(myHash,  current->bucketnodes[j].word) != i)
				printf("erororor\n");
			printf("element:%d is %s  and its key is %d_, ",j, current->bucketnodes[j].word,hash_Function(myHash,  current->bucketnodes[j].word) );
			printf("word %s_ final %d__ cap %d__ added %d", current->bucketnodes[j].word,  current->bucketnodes[j].final,  current->bucketnodes[j].capacity, current->bucketnodes[j].added);
			print_node(current->bucketnodes[j].children, current->bucketnodes[j].word, current->bucketnodes[j].added, 1);
		}
		printf("end bucket %d: with size: %d \n",i,  current->currentsize);
	}
	printf("Hash stats: overflows: %d elements %d realsize %d  roundsize %d pointer %d\n", 
				myHash->overflows, myHash->sumofElements, myHash->maxsize, myHash->currentroundsize, myHash->pointer);
}

void delete_HashInfo(HashInfo* myHash){

	if(myHash == NULL){
		printf("NULL HASH");
		return;
	}
	if(myHash->buckets == NULL){
			printf("NULL buckets");
			return;
	}
	int i,j;
	for(i = 0; i< myHash->maxsize; i++){
		
		Bucket * current= &(myHash->buckets[i]);
		
		if(current!=NULL){
			for(j=0;j < current->currentsize;j++){

				delete_node(&current->bucketnodes[j]);
			}

			if(current->bucketnodes != NULL)
				free(current->bucketnodes);	// free for nodes
		}	 	
	}
	free(myHash->buckets);
	free(myHash);
}

void 	enlarge_bucket(Bucket* bucket){

	bucket->maxsize+= BUCKETSIZE;
	trie_node* enlarged = realloc(bucket->bucketnodes, sizeof(trie_node)*bucket->maxsize);
	if(enlarged == NULL){
		printf("enlarge bucket memory error\n");
		exit(MEMERR);
	}
	memset(&(enlarged[bucket->maxsize - BUCKETSIZE]), 0, sizeof(trie_node)*BUCKETSIZE);
	bucket->bucketnodes = enlarged;
}
