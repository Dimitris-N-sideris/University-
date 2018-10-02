
#include "TopK.h"

topk_node* Topk_init(int type) {

    topk_node* created = malloc(sizeof(topk_node));

    if(created==NULL){
        printf("MEMERROR\n");
        return NULL;
    }

    created->current_batch =0;
    created->number_of_ngrams =0;
    created->topK_num =0;
    created->current_size = TopKStartingSize;
    created->previous_size = TopKStartingSize;
    created->times_size = TopKStartingSize;
    created->num_of_each_time_found = (int *)malloc(TopKStartingSize * sizeof(int));
    memset(created->num_of_each_time_found, 0, sizeof(int) * TopKStartingSize);

    if (type == 1) {

        created->array_of_nodes = (trie_node **) malloc(TopKStartingSize * sizeof(trie_node *));
        memset(created->array_of_nodes, 0, sizeof(trie_node *) * TopKStartingSize);

        created->array_of_ngrams = (char **)malloc(TopKStartingSize * sizeof(char*));
        int i;
        for (i=0; i< TopKStartingSize; i++)
            created->array_of_ngrams[i] = (char *) malloc(MaxNgramSize * sizeof(char));

        created->darray_of_ngrams = NULL;
    }
    else {

        created->darray_of_ngrams = (topk_node_data *)malloc(TopKStartingSize * sizeof(topk_node_data));
        int i;
        for (i=0; i< TopKStartingSize; i++) {

            data_node_reset(&created->darray_of_ngrams[i]);
        }
        created->array_of_nodes = NULL;
        created->array_of_ngrams = NULL;
    }

    return created;
}

int TopK_reset(topk_node* topk, int type) {

    if(topk == NULL) {

        return -1;
    }

    topk->topK_num =0;
    topk->number_of_ngrams =0;
    topk->current_batch++;
    memset(topk->num_of_each_time_found, 0, sizeof(int) * topk->times_size);

    if(type ==  1) {
        memset(topk->array_of_nodes, 0, sizeof(trie_node *) * topk->current_size);

        int i;
        for (i=0; i< topk->current_size; i++)
            topk->array_of_ngrams[i][0] = '\0';
    }
    else {

        int i;
        for (i=0; i< topk->current_size; i++) {

            data_node_reset(&topk->darray_of_ngrams[i]);
        }
    }

    return 0;
}

void TopK_delete (topk_node* topk, int type) {

    free(topk->num_of_each_time_found);

    if(type == 1) {
        free(topk->array_of_nodes);
        int i;
        for (i=0; i< topk->current_size; i++)
            free(topk->array_of_ngrams[i]);

        free(topk->array_of_ngrams);
    }
    else {

        int i;
        for (i=0; i< topk->current_size; i++)
            free(topk->darray_of_ngrams[i].ngram);

        free(topk->darray_of_ngrams);
    }
}

topk_node* TopK_dynamic (topk_node* topk, char* current_ngram) {
    int position = 0;
    int found = 0;
    int previous_times, new_times;
    previous_times = new_times = 0;

    found = topk_binary_search(topk, current_ngram, topk->number_of_ngrams, &position);

    if(found == -1) {

        topk->number_of_ngrams++;

        if(topk->current_size < topk->number_of_ngrams) {
            topk->darray_of_ngrams = enlarge_topk_node(topk->darray_of_ngrams,&topk->current_size);
        }

        if(position != topk->number_of_ngrams-1 && topk->number_of_ngrams-1 != 0) {

            int move = (topk->number_of_ngrams - position - 1)*sizeof(topk_node_data);
            memmove(&topk->darray_of_ngrams[position + 1], &topk->darray_of_ngrams[position], move);
        }

        data_node_reset(&topk->darray_of_ngrams[position]);
        data_copy_word(&topk->darray_of_ngrams[position],current_ngram);
        /*topk->darray_of_ngrams[position].ngram = (char *) malloc(MaxNgramSize * sizeof(char));
        strcpy(topk->darray_of_ngrams[position].ngram, current_ngram);*/
    }

    new_times = topk->darray_of_ngrams[position].times_found + 1;
    previous_times = new_times - 1;
    topk->darray_of_ngrams[position].times_found++;

    if(new_times > topk->times_size) {

        topk->times_size *= 2;
        topk->num_of_each_time_found = enlarge_int_array(topk->num_of_each_time_found,topk->times_size);
    }

    if(topk->num_of_each_time_found[previous_times] > 0)
        topk->num_of_each_time_found[previous_times]--;

    topk->num_of_each_time_found[new_times]++;

    return topk;
}

topk_node* TopK_static (topk_node* topk, trie_node* searchnode, char* current_ngram) {

    if(topk->current_batch > searchnode->current_batch || searchnode->times_found == 0) {
        searchnode->current_batch = topk->current_batch;
        searchnode->string_position = topk->number_of_ngrams;
        searchnode->times_found = 1;
        topk->array_of_nodes = search_change_function(searchnode, topk->array_of_nodes, &topk->number_of_ngrams, &topk->current_size);
        if(topk->previous_size < topk->current_size) {
            topk->array_of_ngrams = enlarge_string_array(topk->array_of_ngrams,&topk->previous_size);
        }
        topk->array_of_ngrams[topk->number_of_ngrams-1][0] = '\0';
        strcpy(topk->array_of_ngrams[topk->number_of_ngrams-1], current_ngram);
        //printf("--------------%s %d\n", topk->array_of_ngrams[topk->number_of_ngrams-1], topk->number_of_ngrams);
    }
    else if (topk->current_batch == searchnode->current_batch) {

        searchnode->times_found++;
    }
    else {

        printf("ERROR IN BATCHES!!!\n");
    }

    return topk;
}

int data_node_reset (topk_node_data* data) {

    data->ngram = NULL;
    data->word_spaceleft= 0;
    data->times_found = 0;

    return 0;
}

void data_copy_word(topk_node_data* currentnode, char* myword){

    int wordsize = strlen(myword) + 1;

    if(currentnode == NULL){
        return;
    }
    if(wordsize > currentnode->word_spaceleft){


        currentnode->ngram = realloc(currentnode->ngram, wordsize + WORDLIMIT);
        if(currentnode->ngram == NULL){

            printf("Function copy word memory error\n");
            exit(MEMERR);
        }

        currentnode->ngram[0] = '\0';
        currentnode->word_spaceleft = WORDLIMIT;
    }
    else{

        currentnode->word_spaceleft -= wordsize;
    }
    strcpy(currentnode->ngram, myword);

}

int count_time_topk (topk_node* topk) {

    int i,times;
    times = 0;

    for ( i = topk->times_size - 1; i >= 0; i--) {

        if (times < topk->topK_num) {

            times += topk->num_of_each_time_found[i];
        }
        else {

            break;
        }
    }

    return times;
}

int topk_binary_search(topk_node* currentnode, char* word,int mysize, int* ordered_position) {

    int small = 0, big = mysize - 1, middle;

    while (small <= big) {

        middle = (big + small) / 2;

        if (strcmp(currentnode->darray_of_ngrams[middle].ngram, word) == 0) {

            *ordered_position = middle;
            return middle; //FOUND
        }
        else if (strcmp(currentnode->darray_of_ngrams[middle].ngram, word) < 0) {

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


char** enlarge_string_array(char** previous_arr, int* maxsize){

    char** newpointer;

    newpointer = realloc(previous_arr, (*maxsize)*2*sizeof(char*));
    if(newpointer == NULL){
        printf("MEMERROR");
        return previous_arr;
    }

    int i;
    for (i= (*maxsize); i< (*maxsize)*2; i++)
        newpointer[i] = (char *)malloc(MaxNgramSize * sizeof(char));

    (*maxsize) *= 2;
    return newpointer;
}

int* enlarge_int_array(int* previous_arr, int newsize){

    int* newpointer;

    newpointer = realloc(previous_arr, newsize*sizeof(int));
    if(newpointer == NULL) {
        printf("MEMERROR");
        return previous_arr;
    }

    int i;
    for (i = newsize/2; i < newsize; i++)
        newpointer[i] = 0;

    return newpointer;
}

topk_node_data*	enlarge_topk_node(topk_node_data* mynode,int* mysize){

    int i, newsize = (*mysize) * 2;

    topk_node_data* enlargednode = realloc(mynode,newsize*sizeof(topk_node_data));

    if(enlargednode == NULL){
        printf("MEMERROR - Enlarge TopK Node");
        return NULL;
    }

    for(i = (*mysize); i < newsize; i++){
        //enlargednode->ngram = NULL;
        enlargednode->times_found=0;
        enlargednode->word_spaceleft=0;
    }
    (*mysize) = newsize;
    return enlargednode;
}

int search_strings (topk_node* topk, char* to_be_found) {

    char** arr_strings = topk->array_of_ngrams;
    int i;

    for(i=0; i<topk->number_of_ngrams; i++) {

        if(strcmp(arr_strings[i],to_be_found) == 0) {

            return i;
        }
    }

    return -1;
}

void swap(trie_node** a, trie_node** b) {
    /* H sunarthsh auth allazei tis thieuthunseis twn 2 deiktwn*/

    trie_node* t = *a;
    *a = *b;
    *b = t;
}

void swap_dynamic(int* times_a, int* times_b, char** a, char** b) {
    /* H sunarthsh auth allazei tis thieuthunseis twn 2 deiktwn*/

    char* t = *a;
    *a = *b;
    *b = t;

    int temp = *times_a;
    *times_a = *times_b;
    *times_b = temp;
}

int compare(int a, int b, char* a_string, char* b_string) {

    int comp =  a - b;

    if (comp < 0)
        return -1;

    if (comp > 0)
        return 1;

    comp = strcmp(b_string, a_string);

    return comp;
}

int partition (topk_node* topk, int left, int right, int type)  {
    /* Auth h sunarthsh pairnei san pivot to teleutaio stoixeio tou pinaka,
       to bazei sthn swsth thesh kai topothtetei ola ta megalutera stoixeia
       aristera tou kai ola ta mikrotera deksia tou*/


    trie_node **arr = topk->array_of_nodes;
    char** strings = topk->array_of_ngrams;

    topk_node_data* dynamic_data = topk->darray_of_ngrams;

    int i = (left - 1);
    int j;

    for (j = left; j <= right - 1; j++) {

        if(type == 1) {
            if (compare(arr[j]->times_found, arr[right]->times_found, strings[arr[j]->string_position], strings[arr[right]->string_position]) > 0) {
                /* Oso to stoixeio einai megalutero tou pivot allakse ta theseis
                   kai aukshse twn deikth gia ta megalutera stoixeia tou pinaka*/
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
        else {

            if (compare(dynamic_data[j].times_found, dynamic_data[right].times_found, dynamic_data[j].ngram, dynamic_data[right].ngram) > 0) {
                /* Oso to stoixeio einai megalutero tou pivot allakse ta theseis
                   kai aukshse twn deikth gia ta megalutera stoixeia tou pinaka*/
                i++;
                swap_dynamic(&dynamic_data[i].times_found, &dynamic_data[j].times_found, &dynamic_data[i].ngram, &dynamic_data[j].ngram);
            }
        }
    }

    if(type == 1) {
        swap(&arr[i + 1], &arr[right]);
    }
    else {
        swap_dynamic(&dynamic_data[i+1].times_found, &dynamic_data[right].times_found, &dynamic_data[i+1].ngram, &dynamic_data[right].ngram);
    }

    return (i + 1);
}

void quicksort(topk_node* topk, int left, int right, int k, int type) {
    /* H sunarthsh auth prokeitai gia mia parallagh ths aplhs quicksort
       sthn opoia kaloume anadromika thn quicksort gia to mikrotero "kommati"
       tou pinaka kai xrhsimopoioume to loop gia na upologisoume to allo miso.
       Etsi panta douleuei me ton mikrotero dunato upopinaka. Auto ginetai giati
       me auton ton tropo o algorithmos kanei sxedon tis mises klhseis apo oti
       mia kanonikh quicksort kai sunepws einai pio grhgoros. */

    int newPivotIdx;

    while (right > left) {

        newPivotIdx = partition(topk, left, right, type);

        if (newPivotIdx - left > right - newPivotIdx) {

            quicksort(topk, newPivotIdx+1, right, k, type);

            right = newPivotIdx - 1;

        }
        else {

            quicksort(topk, left, newPivotIdx-1, k, type);

            left = newPivotIdx + 1;

        }
    }
}

void quicksortGreatestK(topk_node* topk, int left, int right, int k, int type) {
    /* Auth h sunarthsh prokeitai gia to olo "trick" me to opoio pisteuw
       lunetai arketa grhgora kai apodotika to problhma tou sort + topk.
       Genika basistika sthn idea oti an thes na breis to k stoixeio ston
       pinaka ousiastika ksereis kai se poio kommati tou pinaka tha anhkei. */

    int newPivotIdx;

    while (right > left) {

        newPivotIdx = partition(topk, left, right, type);

        if (k <= newPivotIdx) {

            right = newPivotIdx - 1;

        }
        else if (newPivotIdx - left > right - newPivotIdx) {

            quicksort(topk, newPivotIdx+1, right, k, type);

            right = newPivotIdx - 1;

        }
        else {

            quicksort(topk, left, newPivotIdx-1, k, type);

            left = newPivotIdx + 1;

        }
    }
}

void bubblesortGreatestK(topk_node* topk) {

    int i,j, count_k =0;
    int loop_lim =0;
    trie_node** arr = topk->array_of_nodes;
    char** strings = topk->array_of_ngrams;
    bool swapped = FALSE;

    if(topk->topK_num < topk->number_of_ngrams) {

        loop_lim = topk->topK_num;
    }
    else {

        loop_lim = topk->topK_num-1;
    }

    for(i = 0; i < loop_lim; i++) {
        swapped = FALSE;

        for(j = 0; j < loop_lim-i; j++) {

            if(arr[j]->times_found < arr[j+1]->times_found) {
                swap(&arr[j], &arr[j+1]);

                swapped = TRUE;
            }
            if(arr[j]->times_found == arr[j+1]->times_found && strcmp(strings[arr[j]->string_position],strings[arr[j+1]->string_position]) > 0 ) {
                swap(&arr[j], &arr[j+1]);

                swapped = TRUE;
            }

        }
        if(!swapped) {
            break;
        }

        count_k++;
    }
}

void printArray(topk_node* topk, int size, int type) {
    /* Function to print the TopK elements*/

    int i,j;
    trie_node** arr = topk->array_of_nodes;
   // int* times = topk->array_of_times_found;

    if(type == 1) {
        j = arr[0]->string_position;
        char** strings = topk->array_of_ngrams;
        printf("%s", strings[j]);
        for (i=1; i < size; i++) {
            j = arr[i]->string_position;
            printf("|%s", strings[j]);
        }
    }
    else {

        topk_node_data* dynamic_data = topk->darray_of_ngrams;
        printf("%s", dynamic_data[0].ngram);
        for (i=1; i < size; i++) {

            printf("|%s", dynamic_data[i].ngram);
        }
    }

}