#ifndef __Trie__
#define	 __Trie__

#include "header.h"
#include "Ngram.h"
#include "LinearHash.h"
#include "TopK.h"

typedef struct trie_node trie_node;
typedef struct HashInfo  HashInfo;
typedef struct topk_node topk_node;
typedef struct Ngram Ngram;

struct trie_node{

	char* 		word;
	int 		word_spaceleft;	// posos xwros meneis sto char*
	
	int 		final;
	
	short*		wordsizetable;
	int 		tablemaxsize;
	int 		tablecurrentsize;
	
	trie_node* 	children;
	int 		capacity;
	int 		added;

    int         current_batch;      // For static TopK
    int         times_found;
    int         string_position;
    int 		add_version;
    int 		del_version;
};


typedef struct trie{
	
	HashInfo* 	root;
	int 		treesize;
	int 		Ngrams;
	int 		version;

}trie;


//Core Functions
trie* init_Trie();
trie_node* create_trie_node();
	
int 	insert_ngram(trie* ind, Ngram* adding_this);
int 	delete_ngram(trie* ind, Ngram* deleting_this);
//char*	search(trie* ind, Ngram* searching_this);
char*	search(trie* ind, Ngram* searching_this,int* filter,int round, int version);

//static
int 	static_binary_search(trie_node * currentnode, char* word,int mysize, int* ordered_position);
char*   static_search(trie* ind, Ngram* Qgram,int* filter,int round, int version);
void 	combine_nodes(trie_node* target, trie_node* mychild);
int 	compress_node(trie_node* currentnode);
void 	start_compress(trie* ind);

// Helper Functions
trie_node** search_change_function(trie_node* change, trie_node** temp_changes, int* sizeofchanges, int* maxsize);
void 	reset_changes(trie_node** temp_changes, int sizeofchanges);
char*	enlarge_string(char* mystring, int* mysize, char* string_to_insert);
void 	delete_ngram_subfunction(trie* ind,  Ngram* deleting_this, trie_node** deleted_nodes, int * to_be_deleted);
int 	binary_search(trie_node * currentnode, char* word,int mysize, int* ordered_position);
int     bloom_filter(char *bgram,int* filter,int round);


// Extra 
void 	copy_word(trie_node* currentnode, char* myword);
int 	node_reset(trie_node* node);
void 	print_data(trie_node* printing);
void 	print_node(trie_node* printing, char* parentname, int size,int level);
void 	delete_index(trie* ind);
void 	delete_node(trie_node* node);
//char* 	strtok_r(char *str, const char *delim, char **nextp);

#endif
