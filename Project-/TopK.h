

#ifndef __TOP_K__
#define __TOP_K__

#include "trie.h"

#define MaxNgramSize 70
#define TopKStartingSize 1000

typedef struct topk_node topk_node;
typedef struct trie_node trie_node;
typedef struct topk_node_data topk_node_data;

struct topk_node {

    int number_of_ngrams;
    long int topK_num;
    int current_size;
    int times_size;
    int* num_of_each_time_found;
    topk_node_data* darray_of_ngrams;

    int current_batch;          //For static
    int previous_size;
    trie_node** array_of_nodes;
    char** array_of_ngrams;

};

struct topk_node_data {

    char* ngram;
    int word_spaceleft;
    int times_found;
};

//reset initialize and delete TopK
topk_node*  Topk_init(int type);
int         TopK_reset(topk_node* topk, int type);
void        TopK_delete (topk_node* topk, int type);

// TopK Dynamic
topk_node*  TopK_dynamic (topk_node* topk, char* current_ngram);
int         search_strings (topk_node* topk, char* to_be_found);
int         topk_binary_search(topk_node* currentnode, char* word,int mysize, int* ordered_position);
int         data_node_reset (topk_node_data* data);
void        data_copy_word(topk_node_data* currentnode, char* myword);
int         count_time_topk (topk_node* topk);
topk_node_data*	enlarge_topk_node(topk_node_data* mynode,int* mysize);
char**      enlarge_string_array(char** previous_arr, int* maxsize);
int*        enlarge_int_array(int* previous_arr, int newsize);

// Topk Static
topk_node*  TopK_static (topk_node* topk, trie_node* searchnode, char* current_ngram);

void    swap(trie_node** a, trie_node** b);
void    swap_dynamic(int* times_a, int* times_b, char** a, char** b);
int     partition (topk_node* topk, int left, int right, int type);
void    quicksort(topk_node* topk, int left, int right, int k, int type);
void    quicksortGreatestK(topk_node* topk, int left, int right, int k, int type);
int     compare(int a, int b, char* a_string, char* b_string);
void    bubblesortGreatestK(topk_node* topk);
void    printArray(topk_node* topk, int size, int type);


#endif
