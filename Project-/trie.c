#include "trie.h"
#include "murmur3.h"
// creates a node
trie_node* create_trie_node(){

	trie_node* created = malloc(sizeof(trie_node)*STARTNODESIZE);

	if(created==NULL){
		printf("MEMERROR\n");
		return NULL;
	}
	int i;

	for(i = 0; i < STARTNODESIZE; i++){

		node_reset(&created[i]);	
	}
	return created;
}


trie_node*	enlarge_node(trie_node* mynode,int* mysize){

	if(mynode==NULL){	// Node was- uninitialized
	//	printf("no node... creating node\n");
		*mysize = STARTNODESIZE;
		return create_trie_node();
	}
	int i, newsize = (*mysize) * 2;
	
	trie_node* enlargednode = realloc(mynode,newsize*sizeof(trie_node));
		
	if(enlargednode == NULL){
		printf("MEMERROR");
		return NULL;
	}

	for(i = (*mysize); i < newsize; i++){
		enlargednode[i].word = NULL;
		node_reset(&enlargednode[i]);
	}
	(*mysize) = newsize;
	return enlargednode;
}

// creates empty Trie
trie* init_Trie(){

	trie* created = malloc(sizeof(trie));

	if(created==NULL){
		printf("MEMERROR\n");
		return NULL;
	}

	created->root  = create_Hash();

	created->treesize 	= 0;
	created->Ngrams 	= 0;
	created->version	= 0;
	return created;
}


int node_reset(trie_node* node){

	if(node==NULL){

		return -1;
	}

	node->wordsizetable		= NULL;
	node->tablemaxsize 		= 0;
	node->tablecurrentsize 	= 0;

	node->del_version 		= 0;
	node->add_version 		= 0;
	node->word 			 	= NULL;
	node->word_spaceleft 	= 0;
	
	node->final 		 	= FALSE;
	
	node->capacity		 	= 0;
	node->added			 	= 0;
	node->children		 	= NULL;

    node->current_batch     = 0;
    node->times_found       = 0;
    node->string_position   = 0;

    return 0;
}

//
void copy_word(trie_node* currentnode, char* myword){

	int wordsize = strlen(myword) + 1;

	if(currentnode == NULL){
		return;
	}
	if(wordsize > currentnode->word_spaceleft){


		currentnode->word = realloc(currentnode->word, wordsize + WORDLIMIT);
		if(currentnode->word == NULL){

			printf("Function copy word memory error\n");
			exit(MEMERR);
		}

		currentnode->word[0] = '\0';
		currentnode->word_spaceleft = WORDLIMIT;
	}
	else{
			
		currentnode->word_spaceleft -= wordsize;
	}
	strcpy(currentnode->word, myword);

}


int  insert_ngram(trie* ind,Ngram* adding_this){
	
	if(ind==NULL){
		printf("NO TRIE\n");
		return -1;
	}
	
	if(adding_this==NULL){
		printf("NO NGRAM\n");
		return -2;
	}
	
	char * word = adding_this->wordBuffer[0];
	
	int final = FALSE, i, ordered_position, cap_used, maxcap;
	//sleep(1);
	if(adding_this->currentsize == 1){
		final = TRUE;
	}
	

	trie_node  *currentnode = NULL, *parent_node = add_HashInfo(ind->root, word, final, ind->version);

	//currentnode = parent_node->children;
	
	for (i = 1; i < adding_this->currentsize; i++) {

		ordered_position = 0;
		
		word = adding_this->wordBuffer[i];
		
	
		if(parent_node->children == NULL){
			parent_node->children =  enlarge_node(parent_node->children, &(parent_node->capacity));
		}
		currentnode	= parent_node->children;
		maxcap  	= parent_node->capacity;
		cap_used 	= parent_node->added;
		//oldpointer  = &parent_node->children;
	
		if(binary_search(currentnode, word, cap_used, &ordered_position) == -1) {

			if (maxcap == cap_used) {	// if full double
		
				currentnode = enlarge_node(currentnode, &maxcap);
				parent_node->children = currentnode;		//update the "real" pointer
				parent_node->capacity = maxcap;
				
			}

			if (cap_used != ordered_position && cap_used != 0) {

				int move = (cap_used - ordered_position)*sizeof(trie_node);
				memmove(&currentnode[ordered_position + 1], &currentnode[ordered_position], move);	
			}
		//	if(currentnode[ordered_position].word!=NULL) free(currentnode[ordered_position].word);
			node_reset(&currentnode[ordered_position]);
			copy_word(&currentnode[ordered_position], word);
			
			//strcpy(currentnode[ordered_position].word, word);
			parent_node->added++;
			
		}
		if (i + 1 == adding_this->currentsize) {
			if(currentnode[ordered_position].final == FALSE){
				currentnode[ordered_position].add_version = ind->version;
				currentnode[ordered_position].final = TRUE;
			}
		}
		else {

			if (currentnode[ordered_position].children == NULL) {

				currentnode[ordered_position].children = enlarge_node(currentnode[ordered_position].children, &currentnode[ordered_position].capacity);
			}
			parent_node = &currentnode[ordered_position];
		}
	}

	return 0;
}


int binary_search(trie_node * currentnode, char* word,int mysize, int* ordered_position) {

	int small = 0, big = mysize - 1, middle;
	
	while (small <= big) {
	      
		middle = (big + small) / 2;
		
		if (strcmp(currentnode[middle].word, word) == 0) { 			
	      	
			*ordered_position = middle;
			return middle; //FOUND
		}  
		else if (strcmp(currentnode[middle].word, word) < 0) {

			*ordered_position = middle + 1;
			small = middle + 1;
		}
		else {

			*ordered_position = middle;
			big = middle - 1;
		}
	}
	return -1; 	
}


int  delete_ngram(trie* ind, Ngram* deleting_this) {

	if (ind == NULL) {

		return ARG_ERROR;
	}

	if (deleting_this == NULL) {

		return ARG_ERROR;
	}

	//int* to_be_deleted;  // position of deletion targets in each level of the trie
	int level, cap_used, position_found = -1,
		size_of_Ngram = deleting_this->currentsize;

	char* word = deleting_this->wordBuffer[0];	
	trie_node * currentnode = lookup_Hash(ind->root, word);

	if(currentnode == NULL)
		return NOTFOUND;
	
	
	for (level = 1; level < size_of_Ngram; level++) {
		int ordered_position = 0;
		if( currentnode->children == NULL){	// no more nodes stop deleting
			
			return NOTFOUND;
		}
		else{
			cap_used =  currentnode->added;
		}

		word = deleting_this->wordBuffer[level];
	
		position_found = binary_search(currentnode->children, word, cap_used, &ordered_position);
		if (position_found >= 0) { // FOUND
		
			cap_used = currentnode[position_found].added;	//	prepare for next iteration and go down one level
			
			currentnode = &(currentnode->children[position_found]);
			
			//if(currentnode->del_version != 0){
			//	break;
			//}
		}
		else { // Ngram isn't in trie free allocated space and stop delete
			
			return NOTFOUND;
		}
	}
	//to_be_deleted = &position_found;

	if(currentnode->final == TRUE){
		currentnode->del_version = ind->version;
	}
	
	return OK_SUCCESS;
}
// NOT USED IN THREAD VERSION
// whole ngram has been found and will be deleted
void 	delete_ngram_subfunction(trie* ind, Ngram* deleting_this, trie_node** deleted_nodes, int * to_be_deleted) {
	
	int maxlevel = deleting_this->currentsize;
	int  singlewordngram = 1, level = maxlevel - 1;
	//move , last
	for (; level > 0; level--) {	// start from bottom

		singlewordngram = 0;
		int position_found = to_be_deleted[level];
		trie_node * currentnode = deleted_nodes[level];
	// int parent_pos,	trie_node* parentnode = NULL,
		if(level == maxlevel -1){

			if (currentnode[position_found].final == FALSE){

				return;
			}
			else{
				// final stays TRUE  but mark it deleted
				if(currentnode[position_found].del_version == 0){
					currentnode[position_found].del_version = ind->version;
				}
			}
		}
	
		if (currentnode[position_found].added > 0) {	//if node has children dt delete
			// other n grams pass from here can't delete, nor upper level
			// thus deletion stop here
			return;
		}
		else {	
		
			if((currentnode[position_found].final == TRUE) && (level != (maxlevel -1))){
				return;
			}
			/*// mark it deleted as long as it wasn't deleted before
			if(currentnode[position_found].del_version == 0){
					currentnode[position_found].del_version = ind->version;
			}*/
			// prepare for deletion
			/*
			if (currentnode[position_found].children != NULL)			free(currentnode[position_found].children);

			if(currentnode[position_found].wordsizetable != NULL)		free(currentnode[position_found].wordsizetable);

			free(currentnode[position_found].word);
			if(level - 1 == 0){

				parentnode 	=  lookup_Hash(ind->root, deleting_this->wordBuffer[0]);
				last		= --(parentnode->added);		
			}
			else{

				parentnode 	= deleted_nodes[level - 1];
				parent_pos 	= to_be_deleted[level - 1];
				last		= --(parentnode[parent_pos].added);	
			}
			
			move 		= (last - position_found) * sizeof(trie_node);		// how many bytes to shift with memmove
															
			if (move > 0) {	// if deleted element is 
				// print_node(ind->root, "root" , ind->added, 0);
				memmove(&(currentnode[position_found]), &(currentnode[position_found + 1]), move);
				// print_node(ind->root, "root" , ind->added, 0);
			}
			node_reset(&currentnode[last]);*/
		}
	}
	// root case
	deleteFromHash(ind->root, deleting_this->wordBuffer[0], singlewordngram);
}

trie_node** search_change_function(trie_node* change, trie_node** temp_changes, int* sizeofchanges, int* maxsize){
	
	trie_node ** newpointer = temp_changes;
	if((*sizeofchanges) == (*maxsize)){
		newpointer = realloc(temp_changes, (*maxsize)*2*sizeof(trie_node*));
		if(newpointer == NULL){
			printf("MEMERROR");
			return temp_changes;
		}
		(*maxsize) *= 2;
		//*temp_changes = newpointer
	}
	//change->final = FALSE;
	newpointer[*sizeofchanges] = change;
	(*sizeofchanges)++;

	return newpointer;
}

void reset_changes(trie_node** temp_changes, int sizeofchanges){
	int i;
	for(i = 0; i < sizeofchanges; i++){
		temp_changes[i]->final = TRUE;
	}
}


// search helping variables
	
//int maxsize = 2;
//trie_node **temp_changes = NULL;	// table that holds nodes that have been printed
// search helping variables

char* search(trie* ind, Ngram* Qgram,int* filter,int round, int version) {

	int ngramsize = Qgram->currentsize;
	int i, j, k, child_position, ngram_length;
	int ordered_position = 0;

	int result_size = LINELIMIT;
	char* result_string = enlarge_string(NULL, &result_size, NULL);	
	result_string[0] = '\0';
	if(result_string == NULL){
		printf("not enough memory\n");
		exit(MEMERR);
	}

	int  ngram_string_size = LINELIMIT; 
	char* ngram = enlarge_string(NULL, &ngram_string_size, NULL);
	
	if(ngram == NULL){
		printf("not enough memory\n");
		exit(MEMERR);
	}
	ngram[0] = '\0';

	trie_node * searchnode = NULL;

	for (j = 0; j < ngramsize; j++) {

		ngram_length = 0;
		for (i = j; i < ngramsize; i++) {
			
			if(i==j){ // Hash case (root)

				searchnode = lookup_Hash(ind->root,  Qgram->wordBuffer[i]);
			}
			else{	// tree case
				if(searchnode->children == NULL){
					break;
				}
				child_position = binary_search(searchnode->children, Qgram->wordBuffer[i], searchnode->added, &ordered_position);

				if(child_position < 0)	break;

				searchnode = &searchnode->children[child_position];
			}
			
			if (searchnode != NULL) {
				//if(strcmp(searchnode->word, "type")==0)  print_node(searchnode, "type",added, -1);
				
					

				ngram_length++;
				if (searchnode->final == TRUE) {
					
					if(searchnode->add_version > version){
						continue;
					}
					if(searchnode->del_version < version && searchnode->del_version != 0){
						continue;
					}

						ngram[0] = '\0';
						ngram = enlarge_string(ngram,  &ngram_string_size, Qgram->wordBuffer[j]);

						for (k = j+1; k < j + ngram_length; k++) {
					
							ngram = enlarge_string(ngram, &ngram_string_size, Qgram->wordBuffer[k]);								
						}
						ngram[strlen(ngram) - 1] = '\0';

						if(bloom_filter(ngram, filter, round)==1){
							result_string = enlarge_string(result_string,  &result_size, ngram);

							//topk = TopK_static(topk,searchnode,ngram);

							result_string[strlen(result_string) - 1] = '|';
						}
				}
			}	
			else	break;	
		}
	}

	//reset_changes( temp_changes, sizeofchanges);

	if (strlen(result_string) == 0) {

		strcat(result_string, "-1");
	}
	else {

		result_string[strlen(result_string) - 1] = '\0';
	}

	strcat(result_string,"\n");

	if( ngram != NULL){
		free(ngram);
	}
	
	//result string is free'd from threads
			
	return result_string;
}

int bloom_filter(char *bgram,int* filter,int round){
//printf("running new blow");
	uint32_t hash[4];
	uint32_t seed = 42; 
	int i,counter;

	//MurmurHash3_x64_128(bgram, strlen(bgram), seed, hash); //64 bit
	MurmurHash3_x86_128(bgram, strlen(bgram), seed, hash);   //32 bit
	uint32_t s1 = hash[0];
	uint32_t s2 = hash[1];


    counter=0;
    for (i = 0; i < numofhash; i++) {
	  // printf(" bloom %u\n", (s1+i*s2)%700000);    
		if(filter[(s1+i*s2)%FILTERSIZE]!=round){

			filter[(s1+i*s2)%FILTERSIZE]=round;
		}
		else{
			counter++;	
		}
 	}

	if(counter == numofhash){ 	// an oles oi 8eseis einai piasmenes
		return 0;
	}
	else{	//an oxi
		return 1;
	}
}



char*	enlarge_string(char* mystring, int* mysize, char* string_to_insert) {
	char* temp;
	if (mystring == NULL) {
							
		char* mystring = malloc(*mysize * sizeof(char));
		mystring[0] = '\0';
		return mystring;
	}

	if (strlen(mystring) + strlen(string_to_insert) + 3 >= *mysize) {

		int newsize = (strlen(mystring) + strlen(string_to_insert))*2;

		//char* new_string = NULL;

		temp = realloc(mystring, newsize * sizeof(char));

		if (temp == NULL) {

			puts("Error (re)allocating memory for NEW STRING (Enlarge String Function)\n");
			exit(MEMERR);	
		}
		mystring = temp;
		*mysize = newsize;
	}

	strcat(mystring, string_to_insert);
	strcat(mystring, " ");

	return mystring;
}




int static_binary_search(trie_node * currentnode, char* word,int mysize, int* ordered_position) {

	int small = 0, big = mysize - 1, middle;
	char firstword[WORDLIMIT];
	memset(firstword, 0, WORDLIMIT);
	firstword[0] = '\0';

	while (small <= big) {
	      
		middle = (big + small) / 2;
		if(currentnode[middle].wordsizetable == NULL) {
			strcpy(firstword, currentnode[middle].word);
		}
		else{
			int i, wordlength = currentnode[middle].wordsizetable[0];
	
			if(wordlength < 0) 					wordlength *= -1;
	
			for(i = 0; i < wordlength; i++) 	firstword[i] = currentnode[middle].word[i];
	
			firstword[wordlength] = '\0';
		}

		if (strcmp(firstword, word) == 0) { 			
	      	
			*ordered_position = middle;
			return middle; //FOUND
		}  
		else if (strcmp(firstword, word) < 0) {

			*ordered_position = middle + 1;
			small = middle + 1;
		}
		else {

			*ordered_position = middle;
			big = middle - 1;
		}
	}
	return -1; 	
}


//Static Search

char* static_search(trie* ind, Ngram* Qgram,int* filter,int round, int version) {

	int ngramsize = Qgram->currentsize;
	int i, j, k, child_position, ngram_length;
	int ordered_position = 0;

	int result_size = LINELIMIT;
	char* result_string = enlarge_string(NULL, &result_size, NULL);	
	result_string[0] = '\0';
	if(result_string == NULL){
		printf("not enough memory\n");
		exit(MEMERR);
	}

	int  ngram_string_size = LINELIMIT; 
	char* ngram = enlarge_string(NULL, &ngram_string_size, NULL);
	
	if(ngram == NULL){
		printf("not enough memory\n");
		exit(MEMERR);
	}
	ngram[0] = '\0';

	trie_node * searchnode = NULL;
	char 	firstword[WORDLIMIT];

	for (j = 0; j < ngramsize; j++) {

		ngram_length = 0;
		for (i = j; i < ngramsize; i++) {
			
			if(i==j){ // Hash case (root)

				searchnode = static_lookup_Hash(ind->root,  Qgram->wordBuffer[i]);
			}
			else{	// tree case
				if(searchnode->children == NULL){
					break;
				}
				child_position = static_binary_search(searchnode->children, Qgram->wordBuffer[i], searchnode->added, &ordered_position);

				if(child_position < 0)	break;

				searchnode = &searchnode->children[child_position];
			}
			//printf("%s",Qgram->wordBuffer[i]);
			if (searchnode != NULL) {
				
				
				if(searchnode->wordsizetable == NULL){	// hasn't fused with something
					ngram_length++;
					if (searchnode->final == TRUE) {
						
						ngram[0] = '\0';
						ngram = enlarge_string(ngram,  &ngram_string_size, Qgram->wordBuffer[j]);

						for (k = j+1; k < j + ngram_length; k++) {
					
							ngram = enlarge_string(ngram, &ngram_string_size, Qgram->wordBuffer[k]);								
						}
						ngram[strlen(ngram) - 1] = '\0';

						if(bloom_filter(ngram, filter, round)==1){
							
							result_string = enlarge_string(result_string,  &result_size, ngram);
							result_string[strlen(result_string) - 1] = '|';
						}
					}
				}
				else{		// has fused
					int w, low = 0, not_found = 0;
					for(w = 0; w < searchnode->tablecurrentsize; w++){
						ngram_length++;	
						int temp = 0;
						if(w != 0)  temp += searchnode->wordsizetable[w - 1];

						if(temp < 0) 		temp *= -1;
						low += temp;		
						int wordlength = searchnode->wordsizetable[w];
						if(wordlength < 0)  wordlength 	*= -1;

						int t,p = 0;
						for(t = low; t < low + wordlength; t++){

							firstword[p] = searchnode->word[t];
							p++;
						}
		
						firstword[p] = '\0';
						//printf("___%s...%s....low:%d, high:%d, p %d\n", firstword, Qgram->wordBuffer[i + w], low , wordlength, p);
						if(i+w >= ngramsize){
							not_found = 1;
							break;
						}
						  if(strcmp(firstword, Qgram->wordBuffer[i + w]) == 0){
							
								if(searchnode->wordsizetable[w] < 0){ //FINAL

									ngram[0] = '\0';
									ngram = enlarge_string(ngram,  &ngram_string_size, Qgram->wordBuffer[j]);

									for (k = j+1; k < j + ngram_length; k++) {
									
										ngram = enlarge_string(ngram, &ngram_string_size, Qgram->wordBuffer[k]);								
									}
									ngram[strlen(ngram) - 1] = '\0';

									if(bloom_filter(ngram, filter, round) == 1){
											//printf("BLOOM___%s.\n", ngram);
										result_string = enlarge_string(result_string,  &result_size, ngram);
	                                       // topk = TopK_static(topk,searchnode,ngram);

	                                    result_string[strlen(result_string) - 1] = '|';
									}
										
								}
								
						}
						else {
							not_found = 1;
							break;
						}
					}

					if(not_found == 1) break;
					else 	i+= w - 1;

				}
			}	
			else	break;	
		}
	}

	//reset_changes( temp_changes, sizeofchanges);

	if (strlen(result_string) == 0) {

		strcat(result_string, "-1");
	}
	else {

		result_string[strlen(result_string) - 1] = '\0';
	}

	strcat(result_string,"\n");
	if( ngram != NULL){
		free(ngram);
	}
	
	//result string is free'd from threads

	return result_string;
}


// Start Static compression
void start_compress(trie* ind){

	HashInfo* myHash = ind->root;
	int i,j;
	if(myHash == NULL){

		exit(NOTFOUND);
	}
	//int flag = 0,changed;
	//trie_node* temp2 = NULL;
	for(i = 0; i < myHash->maxsize; i++){
		
		Bucket* temp = &myHash->buckets[i];
			
		for(j = 0; j < temp->currentsize; j++){

			compress_node(&(temp->bucketnodes[j]));
		}
	}
	
}

// for each node try to compress until u reach leaves
int compress_node(trie_node* currentnode){

	
	if(currentnode != NULL){

		while(currentnode->added == 1){ // if node has only 1 child node compress them
			
			trie_node* mychild =  currentnode->children;
			combine_nodes(currentnode, &(mychild[0]));
			// parent takes child's data
			currentnode->capacity 	= mychild[0].capacity;
			currentnode->added 		= mychild[0].added;
			currentnode->final 		= mychild[0].final;
			currentnode->children 	= mychild[0].children;

			free(mychild[0].word);
			free(mychild);	
			
		}
		
		int i = 0;
		for(; i < currentnode->added; i++)   compress_node(&(currentnode->children[i]));
		
	}

	return 0;
}

// function compining string
void combine_nodes(trie_node* target, trie_node* mychild){

	if(target == NULL || mychild == NULL){
		printf("NULL POINTER COMBINE\n");
		exit(NOTFOUND);
	}
	
	int wordsize 	= strlen(mychild->word) + 1;
	int targetsize 	= strlen(target->word);
	int flag	  	= FALSE, sign = 1;
	if(target->tablemaxsize == target->tablecurrentsize){	//size table needs change
		
		if(target->tablemaxsize == 0){	// first time compressing 

			flag = TRUE;
		}

		target->wordsizetable = realloc(target->wordsizetable, (target->tablemaxsize + TABLESIZE)*sizeof(short));
		if(target->wordsizetable == NULL){

			printf("Memory error combine\n");
			exit(MEMERR);
		}
		target->tablemaxsize += TABLESIZE;
	}
	if(flag == TRUE){	// if first time put first word inside size table

		if(target->final == TRUE){
			sign = -1;
		}
		target->wordsizetable[target->tablecurrentsize] = targetsize*sign;
		target->tablecurrentsize++;
		sign = 1;
	}
	
	if(wordsize > target->word_spaceleft){	// if word doesn't fit

		target->word = realloc(target->word, wordsize + targetsize + WORDLIMIT);
		if(target->word == NULL){

			printf("Function copy word memory error\n");
			exit(MEMERR);
		}

		target->word_spaceleft = WORDLIMIT;
	}
	else{
			
		target->word_spaceleft -= wordsize;
	}
	
	strcat(target->word, mychild->word);
	if(mychild->final == TRUE){

			sign = -1;
	}
	target->wordsizetable[target->tablecurrentsize] = (wordsize-1)*sign;
	
	target->tablecurrentsize++;
}


void print_data(trie_node* printing){
	if(printing!=NULL){

		printf("word %s_ final %d__ cap %d__ added %d", printing->word, printing->final, printing->capacity, printing->added);
		if(printing->children == NULL){

			printf("NULL children\n");
		}
		else{

			printf("has children\n");
		}
		if(printing->wordsizetable != NULL){
			int i;
			for(i = 0; i < printing->tablecurrentsize; i++)	printf("..I; table pos%d, has size %d..", i, printing->wordsizetable[i]);
		}
	}
}


void print_node(trie_node* printing, char* parentname, int size, int level){

	int i;
	if(printing == NULL)	return;

	printf("level is %d and parent:%s\n",level, parentname);

	for(i=0;i<size;i++)		print_data(&printing[i]);

	printf("\n");

	for(i=0;i<size;i++)		print_node(printing[i].children, printing[i].word, printing[i].added, level+1);

	printf("\n");
}

// helps in destruction of trie different than delete_ngram
void delete_node(trie_node* node) {

	if (node != NULL) {

		int i = 0;
		for (; i < node->capacity; i++) 		delete_node(&(node->children[i]));

		if(node->word != NULL) 				free(node->word);

		if(node->wordsizetable != NULL) 	free(node->wordsizetable);

		free(node->children);
	}
}

// destroys whole trie
void delete_index(trie* ind) {
	if (ind != NULL) {
		delete_HashInfo(ind->root);
		free(ind);
	}
}
/*
char* strtok_r(char *str, const char *delim, char **nextp) {
    char *ret;

    if (str == NULL)
    {
        str = *nextp;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        return NULL;
    }

    ret = str;

    str += strcspn(str, delim);

    if (*str)
    {
        *str++ = '\0';
    }

    *nextp = str;

    return ret;
}*/