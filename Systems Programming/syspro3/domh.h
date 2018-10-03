#include 	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include  	<unistd.h>
#include 	<sys/socket.h>
#include 	<sys/types.h>
#include	<arpa/inet.h>
#include	<netinet/in.h>
#include 	<netdb.h>
#include	<pthread.h>

#define 	KEYVARIABLE		71
#define 	MARK	 		5
#define		CHARLIMIT		1024
#define 	MULT_TRANSFER	"add_multi_transfer"
#define		ADD_ACCOUNT		"add_account"
#define		PRINT_BAL		"print_balance"
#define		PRINT_MULT		"print_multi_balance"
#define		TRANSFER 		"add_transfer"
#define		EXIT 			"exit"
#define		SLEEP			"sleep"
#define 	HASHSIZE 		1000
#define 	LOCKS 			10


typedef struct client{
	int fileDesc;
	struct client* next;
}client;



typedef struct sunallages{
	
	char				client2[CHARLIMIT];
	int 				amount;
	struct sunallages*  next;

}sunallages;


typedef struct komvos{
	
	char				client_name[CHARLIMIT];
	int 				deposit;
	struct sunallages*  list;
	struct komvos*  	next;

}komvos;

typedef struct Headkomvos{
	int 		size;
	komvos* 	arxi;

}Headkomvos;

typedef struct Hashinfo{
	
	int hashsize;
	struct Headkomvos * hashtable;

	int locks;
	pthread_mutex_t* hashlock;

	pthread_mutex_t  queuelock;
	int sizeofQueue;
	int max_clients;
	client*	clients;

	pthread_cond_t	queueFull;
	pthread_cond_t	queuempty;

}Hashinfo;


int pushClient(Hashinfo* hash,int fileDsc);
int popClient(Hashinfo* hash);
int lock_hash(Hashinfo* hash,sunallages* list);
int unlock_hash(Hashinfo* hash,sunallages* list);


Hashinfo* createHash(int size,int locks,int max_queue);

int 	addTransfer(Hashinfo* table,sunallages* name_list, int amount,int size,int delay);
int 	getBalance(Hashinfo* table,sunallages* name_list, char* buffer,int delay);
int 	add_accountHash(Hashinfo* table,char* name, int amount,int delay);
int 	search(Hashinfo* table,sunallages* name_list);
komvos * getFromHash(Headkomvos * bucket,char * name_target);

int 	keyFunction(char* name,int Hashsize);
int 	add_account(Headkomvos* head,char* name,int amount);
int 	comparekey(char* bigger,char* smaller,int size);

int 	addTransferFromList(Hashinfo* table,sunallages* name_list, int amount,int sizeofTransfer);//
int 	addTransaction(komvos* source, komvos * target, int new_amount);///
int 	makeTransaction(komvos* source, komvos * target, int amount);//
int 	addsunallagh(sunallages ** target,char *source_name,int new_amount,int size); 


sunallages* getFirst(sunallages* name_list,int target);
komvos * getElement(Hashinfo* table,char* target);

int 	add_amount(Hashinfo* table,sunallages* name_list, int amount);
int 	add_name(sunallages* name_list, char* buffer);
int 	isNum(char* numerable);

void 	print_Hash(Hashinfo* table);
void 	print_sunallages(sunallages* list,int size);
void 	delete_(sunallages* list);
void 	initialise_Head(Headkomvos* head);
void 	unMarkAll(sunallages* name_list);
