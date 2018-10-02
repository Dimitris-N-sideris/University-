/* file minunit_example.c */
#include "header.h"
#include "trie.h"
#include "LinearHash.h"
#include "unitest.h"
 
int tests_run = 0;
 
int foo = 7;
int bar = 4;
int key = 5;
Ngram *  	myNgram;
HashInfo*	myHash;
trie* 		ind;

 static char * creation() {
 	
 	ind = init_Trie();
 	myHash = create_Hash();
 	createNgram(&myNgram);

    mu_assert("error,Trie test failed\n", myNgram->maxCapacity == STARTINGSIZE);
    return 0;
 }
 static char * test_Ngram() {
 	addinNgram(myNgram,"the");	// 0
 	addinNgram(myNgram,"cat");	// 1
 	addinNgram(myNgram,"is"); 	// 2
 	addinNgram(myNgram,"blue");	// 3
    mu_assert("error,Ngram test failed\n", strcmp(myNgram->wordBuffer[2], "is")==0 );
    return 0;
 }


 static char * test_trie() {
 	insert_ngram(ind, myNgram);
 	resetNgram(myNgram);
 	addinNgram(myNgram,"blue");	// 0
 	insert_ngram(ind, myNgram);
 	resetNgram(myNgram);
 	addinNgram(myNgram,"cat");	// 1
 	insert_ngram(ind, myNgram);
 	resetNgram(myNgram);
 	addinNgram(myNgram,"blue");	// 0
 	addinNgram(myNgram,"cat");	// 1
 	char* temp =	search(ind, myNgram);
    mu_assert("error,Trie test 1 failed\n", strcmp(temp, "blue|cat\n") == 0 );
    return 0;
 }

 static char * test_trie2() {
 
 	resetNgram(myNgram);
 	addinNgram(myNgram,"cat");	// 1
 	delete_ngram(ind, myNgram);
	resetNgram(myNgram);

 	addinNgram(myNgram,"blue");	// 0
 	addinNgram(myNgram,"cat");	// 1
 	char* temp =	search(ind, myNgram);

    mu_assert("error,Trie test 2 failed\n", strcmp(temp, "blue\n") == 0 );
    return 0;
 }


 static char * test_trie3() {

 	resetNgram(myNgram);
 	addinNgram(myNgram,"cat");	// 1
 	insert_ngram(ind, myNgram);
 	resetNgram(myNgram);

 	addinNgram(myNgram,"blue");	// 0
 	addinNgram(myNgram,"cat");	// 1
 	delete_ngram(ind, myNgram);

 	char* temp =	search(ind, myNgram);
    mu_assert("error,Trie test 3 failed\n", strcmp(temp, "blue|cat\n") == 0 );
    return 0;
 }



 static char * test_LinearHash() {
 	add_HashInfo( myHash, "dog", 1);
 	add_HashInfo( myHash, "dog", 1);
 	add_HashInfo( myHash, "the", 1);
 	add_HashInfo( myHash, "gug", 1);
 	trie_node* temp =  lookup_Hash(myHash, "dog");
    mu_assert("error,Linear Hash test failed\n", (strcmp(temp->word, "dog")==0) && (temp->final == 1));
    return 0;
 }

  static char * test_LinearHash2() {
 
 	deleteFromHash( myHash, "dog", 1);
 	trie_node* temp =  lookup_Hash(myHash, "dog");
    mu_assert("error,Linear Hash test 2 failed\n", temp == NULL);
    return 0;
 }
 
 static char * all_tests() {

 	mu_run_test(creation);
    mu_run_test(test_Ngram);
    mu_run_test(test_LinearHash);
    mu_run_test(test_LinearHash2);
    mu_run_test(test_trie);
    mu_run_test(test_trie2);
    mu_run_test(test_trie3);
    return 0;
 }
 
 int main(int argc, char **argv) {
     char *result = all_tests();

     if (result != 0) {
         printf("%s\n", result);
     }
     else {
         printf("ALL TESTS PASSED\n");
     }
     printf("Tests run: %d\n", tests_run);
 
     return result != 0;
 }