#include "domh.h"

int 	isNum(char* numerable){
	int i=0;
	if(numerable==NULL){
		return 0;
	}
	while(numerable[i]!='\0'){
		if(numerable[i]<'0' || numerable[i] >'9'){
			return 0;
		}
		i++;
	}
	return 1;
}

int lock_hash(Hashinfo* hash,sunallages* list){
	int * lock_toget=(int *)calloc(hash->locks,sizeof(int));
	int i;
	if(lock_toget==NULL){
		return -1;
	}
	sunallages* temp=list;
	int bucketperlock=hash->hashsize/hash->locks;
	while(temp!=NULL){
		int key=keyFunction(temp->client2,hash->hashsize);
		lock_toget[key/bucketperlock]=1;
		temp=temp->next;
	}
	for(i=0;i<hash->locks;i++){
		if(lock_toget[i]==1){// lock
			pthread_mutex_lock(&(hash->hashlock[i]));
		}
	}
	free(lock_toget);
	return 0;
}

int unlock_hash(Hashinfo* hash,sunallages* list){
	int * lock_torelease=(int *)calloc(hash->locks,sizeof(int));
	int i;
	if(lock_torelease==NULL){
		return -1;
	}
	sunallages* temp=list;
	int bucketperlock=hash->hashsize/hash->locks;
	while(temp!=NULL){
		int key=keyFunction(temp->client2,hash->hashsize);
		lock_torelease[key/bucketperlock]=1;
		temp=temp->next;
	}
	for(i=0;i<hash->locks;i++){
		if(lock_torelease[i]==1){// lock
			pthread_mutex_unlock(&(hash->hashlock[i]));
		}
	}
	free(lock_torelease);
	return 0;
}

int pushClient(Hashinfo* hash,int fileDsc){
	
	pthread_mutex_lock(&(hash->queuelock));
	client * temp=hash->clients;
	while(hash->sizeofQueue>=hash->max_clients){
		pthread_cond_wait(&(hash->queueFull),&(hash->queuelock));
	}
	client* neoskomvos=malloc(sizeof(client));
	if(neoskomvos==NULL){
		pthread_mutex_unlock(&(hash->queuelock));
		return -1;
	}
	
	neoskomvos->fileDesc=fileDsc;
	neoskomvos->next=NULL;
	hash->sizeofQueue++;
	
	if(temp==NULL){
		pthread_mutex_unlock(&(hash->queuelock));
		pthread_cond_signal(&(hash->queuempty));
		hash->clients=neoskomvos;
		return 0;
	}
	
	while(temp->next!=NULL){
		temp=temp->next;
	}
	temp->next=neoskomvos;

	pthread_mutex_unlock(&(hash->queuelock));
	pthread_cond_signal(&(hash->queuempty));

	
	return 0;
}

int popClient(Hashinfo* hash){
	pthread_mutex_lock(&(hash->queuelock));
	while(hash->sizeofQueue==0){
		pthread_cond_wait(&(hash->queuempty),&(hash->queuelock));
	}
	client * temp=hash->clients;
	int first=temp->fileDesc;
	hash->clients=temp->next;
	free(temp);
	hash->sizeofQueue--;
	if(hash->sizeofQueue==0){
		hash->clients=NULL;
	}
	pthread_mutex_unlock(&(hash->queuelock));
	pthread_cond_signal(&(hash->queueFull));
	return first;
}


Hashinfo* createHash(int size,int lock,int max_queue){			// creates hash with number of locks
	Hashinfo* temp= malloc(sizeof(Hashinfo));
	if(temp==NULL){
		printf("error with malloc\n");
		return NULL;
	}
	else{
		temp->hashtable=malloc(sizeof(Headkomvos)*size);
		if(temp->hashtable==NULL){
			free(temp);
			return NULL;
		}
		int i;
		for(i=0;i<size;i++){
			initialise_Head(&(temp->hashtable[i]));
		}
		if(pthread_cond_init(&(temp->queueFull),NULL)!=0 || pthread_cond_init(&(temp->queuempty),NULL)!=0){
			perror("error with conditions\n");
			return NULL;
		}
		temp->hashsize=size;
		temp->locks=lock;
		temp->sizeofQueue=0;
		temp->max_clients=max_queue;
		temp->clients=NULL;
		if (pthread_mutex_init(&(temp->queuelock), NULL) != 0){
	        printf("mutex init failed\n");
	        return NULL;
	    }
		temp->hashlock=malloc(sizeof(pthread_mutex_t)*lock);
		if(temp->hashlock==NULL){
			printf("malloc fail\n");
			return NULL;
		}
		for(i=0;i<lock;i++){
			if (pthread_mutex_init(&(temp->hashlock[i]), NULL) != 0){
		        printf("mutex init failed\n");
		        return NULL;
	   		}
		}
	}
	return temp;
}


void initialise_Head(Headkomvos* head){			// initialise list of hash
	head->size=0;
	head->arxi=NULL;
}

int keyFunction(char* name,int Hashsize){		// key function of hash
	int i=0,sum=0;
	if(name==NULL){
		return -1;
	}
	while(name[i]!='\0'){
		sum+=name[i];
		i++;
	}
	//printf("MADE STRING TO NUM\n");
	while(sum<Hashsize){
		sum*=KEYVARIABLE;
	}
	return sum%Hashsize;
}

int add_accountHash(Hashinfo* table,char* name, int amount,int delay){		// adds an account in Hash
	if(table==NULL){
		printf("Hashtable not found\n");
		return -1;
	}
	int pos=keyFunction(name,table->hashsize);
	if(pos==-1){
		printf("name=NULL");
		return -1;
	}
	int toLock=pos/(table->hashsize/table->locks);
	pthread_mutex_lock(&(table->hashlock[toLock]));
	usleep(delay);
	if(getFromHash(&(table->hashtable[pos]),name)==NULL){	// try to get before adding to check for duplicates
		
		int err=add_account(&(table->hashtable[pos]),name,amount);
		pthread_mutex_unlock(&(table->hashlock[toLock]));
		return err;
	}
	pthread_mutex_unlock(&(table->hashlock[toLock]));
	return -2;							// try to get before adding to check for duplicates
	
	
}

int add_account(Headkomvos* head,char* name,int amount){	// adds an account in hash list

	komvos* temp=malloc(sizeof(komvos));
	if(temp==NULL){
		return -1;
	}
	strcpy(temp->client_name,name);
	temp->deposit=amount;
	temp->list=NULL;
	temp->next=head->arxi;
	head->arxi=temp;
	return 0;
}

komvos * getElement(Hashinfo* table,char* target){		// get element from hash
	int key=keyFunction(target,table->hashsize);
	if(key<0){
		printf("error with name in search\n");
		return NULL;
	}
	return getFromHash(&(table->hashtable[key]),target);	// name not found		
}

	
komvos * getFromHash( Headkomvos * bucket,char * name_target){	// gets element from Hash list
	komvos* temp= bucket->arxi;
	while(temp!=NULL){
		if(strcmp(temp->client_name,name_target)==0){
			return temp;
		}
		temp=temp->next;
	}
	return NULL;
}

int search(Hashinfo* table,sunallages* name_list){			// psaxnei sto hash an uparxoun ta onomata ths name list
	int source=0,dest=0,key;
	if(table==NULL||name_list==NULL){
		printf("error with search\n");
		return -1;
	}
	sunallages* temp= name_list;
	while(temp!=NULL){
		

		key=keyFunction(temp->client2,table->hashsize);
		if(key<0){
			printf("error with name in search\n");
			return -1;
		}
		if(getFromHash(&(table->hashtable[key]),temp->client2)==NULL){	// name not found
			return -2;
		}
		else{
			if(temp->amount==1){
				source+=1;
			}
			else if(temp->amount==0){
				dest+=1;
			}
		}
		temp=temp->next;
	}
	if(source==0|| dest < 1){ // error with command
		return -3;
	}
	return 0;
}


sunallages* getFirst(sunallages* name_list,int target){		// pairnei to prwto element me amount = target
	sunallages* temp=name_list;
	sunallages* found=NULL;
	if(temp==NULL){
		return NULL;
	}
	while(temp!=NULL){
		if(temp->amount==target){
			found=temp;
			temp->amount-=MARK;											// markarei to amount gia na mn to 3anavrei meta
			return found;
		}
		temp=temp->next;
	}
	return found;
}


int comparekey(char* bigger,char* smaller,int size){				// sunarthsh sugkrishs kleidiwn 2 le3ewn
	int key_bigger,key_smaller;
	key_bigger=keyFunction(bigger,size);
	key_smaller=keyFunction(smaller,size);
	if(key_bigger>key_smaller){
		return 1;
	}
	else if(key_bigger==key_smaller){
		return 0;
	}
	else{
		return -1;
	}
}

int addsunallagh(sunallages ** target,char *source_name,int new_amount,int size){ 	// vale nea sunallagh sth lista tou logariasmou target 
	sunallages * temp=*target;
	sunallages* neoskomvos;
	if(temp==NULL){
		*target=malloc(sizeof(sunallages));
		if(*target==NULL){
			return -1;
		}
		(*target)->amount=new_amount;
		strcpy((*target)->client2,source_name);
		(*target)->next=NULL;
		return 0;
	}

	if(comparekey(temp->client2,source_name,size)>0){
		neoskomvos=malloc(sizeof(komvos));
		if(neoskomvos==NULL){
			return -1;
		}
		strcpy(neoskomvos->client2,source_name);
		neoskomvos->amount=new_amount;
		neoskomvos->next=*target;
		*target=neoskomvos;
		return 0;
	}

	while(temp->next!=NULL){
	
		if(comparekey(temp->next->client2,source_name,size)>0){
			break;
		}
		temp=temp->next;
	}
	
	neoskomvos=malloc(sizeof(komvos));
	if(neoskomvos==NULL){
		return -1;
	}
	strcpy(neoskomvos->client2,source_name);
	neoskomvos->amount=new_amount;
	neoskomvos->next=temp->next;
	temp->next=neoskomvos;
	
	return 0;
}

int addTransaction(komvos* source, komvos * target, int new_amount){
	
	if(target==NULL || source==NULL){
		printf("wrong komvoi\n");
		return -1;
	}
	sunallages * temp=target->list;
	if(temp==NULL){
		target->list=malloc(sizeof(sunallages));
		if(target->list==NULL){
			return -1;
		}
		target->list->amount=new_amount;
		strcpy(target->list->client2,source->client_name);
		target->list->next=NULL;
		return 0;
	}

	while(temp->next!=NULL){
		if(strcmp(temp->client2,source->client_name)==0){
			temp->amount+=new_amount;
			return 0;
		}
		temp=temp->next;
	}
	if(strcmp(temp->client2,source->client_name)==0){
		temp->amount+=new_amount;
		return 0;
	}
	sunallages* neoskomvos=malloc(sizeof(komvos));
	if(neoskomvos==NULL){
		return -1;
	}
	strcpy(neoskomvos->client2,source->client_name);
	neoskomvos->amount=new_amount;
	neoskomvos->next=NULL;
	temp->next=neoskomvos;

	return 0;
}

void unMarkAll(sunallages* name_list){
	sunallages*temp= name_list;
	while(temp!=NULL){
		temp->amount+=MARK;
		temp=temp->next;
	}
}

int addTransferFromList(Hashinfo* table,sunallages* name_list, int amount,int sizeofTransfer){
	int source_key, dest_key;
	komvos* source_client,*target_client;
	sunallages* source= getFirst(name_list,1);
	if(source==NULL){
		printf("error source\n");
		return -1;
	}
	source_key=keyFunction(source->client2,table->hashsize);		// get the source
	source_client=getFromHash(&(table->hashtable[source_key]),source->client2);
	int amountTransfered=sizeofTransfer*amount;
	if(source_client->deposit<amountTransfered){		// not enough money
		
		return -4;
	}
	while(sizeofTransfer>0){
		sunallages* dest= getFirst(name_list,0);
		sizeofTransfer--;
		if(dest==NULL){
			printf("error target\n");
			//print_sunallages(name_list,table->hashsize);
			continue;
		}
		dest_key=keyFunction(dest->client2,table->hashsize);		// get the target

		target_client=getFromHash(&(table->hashtable[dest_key]),dest->client2);
		
		if(addTransaction(source_client,target_client,amount)==-1){
			printf("bad Transaction\n");
			return -1;
		}
		target_client->deposit+=amount;

	}
	//unMarkAll(name_list);
	//add_amount(table,name_list, amount);
	source_client->deposit-=amountTransfered;

	return 0;
}
int add_amount(Hashinfo* table,sunallages* name_list, int amount){
	int key;
	if(name_list==NULL){
		return -1;
	}
	sunallages* temp= getFirst(name_list,0);

	while(temp!=NULL){
		key=keyFunction(temp->client2,table->hashsize);
		komvos* target_client=getFromHash(&(table->hashtable[key]),temp->client2);
		target_client->deposit+=amount;
		temp= getFirst(name_list,0);
	}
	return 0;
}
int addTransfer(Hashinfo* table,sunallages* name_list, int amount,int size,int delay){
	int dest_key;

	if(table==NULL){
		printf("Hashtable not found\n");
		return -1;
	}
	lock_hash(table,name_list);
	usleep(delay);
	if((dest_key=search(table,name_list))<0){
		unlock_hash(table,name_list);
		return dest_key;
	}

	
	if(addTransferFromList( table,name_list,amount,size)<0){
			unlock_hash(table,name_list);
		return -1;
	}
	unlock_hash(table,name_list);
	return 0;
}

// print to be built
int getBalance(Hashinfo* table,sunallages* name_list, char* buffer,int delay){
	int key;
	komvos* client;
	int pos=strlen(buffer);
	lock_hash(table,name_list);	// lock hash for the specific list
	usleep(delay);
	sunallages* target=name_list;

	pos+=sprintf(buffer+pos,"(");
	while(target!=NULL){
		if((key=keyFunction(target->client2,table->hashsize))<0){
			//printf("key Client %s wasn't found in Hash",target->client2);
			unlock_hash(table,name_list);
			return -1;
		}
		client=getFromHash(&(table->hashtable[key]),target->client2);
		if(client==NULL){
			//printf("Client %s wasn't found in Hash",target->client2);
			unlock_hash(table,name_list);
			return -2;
		}
		pos+=sprintf(buffer+pos,"%s/%d",client->client_name,client->deposit);
		if(target->next!=NULL){
				pos+=sprintf(buffer+pos,":");
		}
		target=target->next;
	}
	
	sprintf(buffer+pos,")");
	unlock_hash(table,name_list);
	return 0;
}
 
int add_name(sunallages* name_list, char* buffer){
	sunallages* temp=name_list;
	int pos;

	while(temp!=NULL){
		pos=strlen(buffer);
		sprintf(buffer+pos," %s",temp->client2);

		temp=temp->next;
		if(temp!=NULL){
			pos=strlen(buffer);
			sprintf(buffer+pos,":");
		}
	}
	return 0;
}
void print_sunallages(sunallages* list,int size){
	sunallages* temp= list;
	while(temp!=NULL){
		int key=keyFunction(temp->client2,size);
		printf("%s %d %d:\n",temp->client2,temp->amount,key);
		temp=temp->next;
	}
}

void print_Hash(Hashinfo* table){
	int i=0;
	komvos* temp;
	for(i=0;i<table->hashsize;i++){
		temp=table->hashtable[i].arxi;
		
		while(temp!=NULL){
			printf("%d:\n",i);
			printf("%s %d\n",temp->client_name,temp->deposit);
			print_sunallages(temp->list,table->hashsize);
			temp=temp->next;

		}
	}

}



void delete_(sunallages* list){
	if(list!=NULL){
		delete_(list->next);
		free(list);
	}
}