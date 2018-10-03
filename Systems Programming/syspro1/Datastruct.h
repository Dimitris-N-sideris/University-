#ifndef __DATASTRUCT_
#define __DATASTRUCT_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <bool.h>
#define CHARLIMIT 1024
// orio gia ta char

typedef struct akmh{
	struct graphNode* 		nodeptr;
	double 					poso;	
	struct akmh*			next;
}akmh;


typedef struct lista_akmwn{
	akmh*			arxi;
	int 			size;
}lista_akmwn;

typedef struct graphNode{
	int 			keyName;
	double 			income;
	double 			outcome; 
	int 			check;
	lista_akmwn*	payments;
}graphNode;




int 		add_akmh(graphNode * source, double poso,graphNode* data);
int 		remove_akmh(graphNode * source,graphNode* target);
void 		delete_all(akmh* target);
int 		delete_Node(graphNode* target);
graphNode*	create_graphNode(int id);

lista_akmwn*create_list();
int 		empty_list(graphNode* node);

int 		addakmh(lista_akmwn* list,graphNode* data);
int  		removeakmh(lista_akmwn* list,graphNode* data);
void 		printList(lista_akmwn* list);

double		lookup(graphNode* node,int mode);
void 		print_data(FILE* stream,graphNode* node);
int 		pathfind(graphNode* current,graphNode* target,lista_akmwn* list);		// finds one path to "target" and prints it
int 		printcycles(graphNode* current,graphNode*target,lista_akmwn* list);		// finds some circles size>2 and prints them
int 		path(graphNode* current,lista_akmwn* list,int leftover,double amount);	// finds all paths size==leftover and prints them
#endif

