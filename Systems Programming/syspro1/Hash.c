#include "Hash.h"


Hashinfo* hashCreation(int hashsize){
	
	Hashinfo* temp=malloc(sizeof(Hashinfo));
	if(temp==NULL){
		fprintf(stderr, "error with Hash malloc\n");
		return NULL;
	}
	
	temp->table=malloc(sizeof(Bucket)*hashsize);
	if(temp->table==NULL){
		fprintf(stderr, "error with table malloc\n");
		return NULL;
	}
	temp->size=hashsize;
	int j,i=0;
	for(;i<hashsize;i++){
		temp->table[i].next=NULL;
		for(j=0;j<BUCKETSIZ;j++){
			temp->table[i].komvoi[j]=NULL;
		}
	}
	return temp;
}

int keyfunction(Hashinfo* hash,graphNode* target){

	return (target->keyName)%(hash->size);
}

int addElement(Bucket* target,graphNode* data){	// vazei  stoixeio
	int i=0;
	Bucket* temp=target;
	do{
		for(i=0;i<BUCKETSIZ;i++){
			if(temp->komvoi[i]==NULL){
				temp->komvoi[i]=data;
				return 0;
			}
		}
		target=temp;
		temp=temp->next;
	}while(temp!=NULL);
	return addBucket(target,data);
}

int addBucket(Bucket* target,graphNode* data){
	Bucket* temp=malloc(sizeof(Bucket));
	if(temp==NULL){
		return 1;
	}
	temp->next=NULL;
	int j=1;
	temp->komvoi[0]=data;
	for(;j<BUCKETSIZ;j++){
		temp->komvoi[j]=NULL;
	}
	target->next=temp;
	return 0;
}

int insertHash(Hashinfo* hash,graphNode* element){
	
	if(searchHash(hash,element->keyName)!=NULL){
		printf("Insert failed: Node with %d already in Hash",element->keyName);
		return 1;
	}
	int pos=keyfunction(hash,element);
	if(addElement(&(hash->table[pos]),element)!=0){
		return 1; 	//failed
	}
	hash->elements++;
	return 0;
}

graphNode* 	removefromhash(Hashinfo* hash,int id){
	int i,pos=id%hash->size;
	Bucket* temp=&hash->table[pos];
	graphNode* target=NULL;
	while(temp!=NULL){
		for(i=0;i<BUCKETSIZ;i++){
			if(temp->komvoi[i]==NULL){
				continue;
			}
			if(temp->komvoi[i]->keyName==id){
				target=temp->komvoi[i];
				temp->komvoi[i]=NULL;
				return target;
			}
		}
		temp=temp->next;
	}
	return NULL;
}



graphNode* searchHash(Hashinfo* hash,int id){
	int i,pos=id%hash->size;
	Bucket* temp=&hash->table[pos];
	
	while(temp!=NULL){
		for(i=0;i<BUCKETSIZ;i++){
			if(temp->komvoi[i]==NULL){
				continue;
			}
			if(temp->komvoi[i]->keyName==id){
				return temp->komvoi[i];
			}
		}
		temp=temp->next;
	}
	return NULL;
}

int runthroughHash(Hashinfo* hash){	// goes to every entry of the hash table
	int i=0;
	Bucket * temp=NULL;
	for(;i<hash->size;i++){
		temp=&hash->table[i];
		while(temp!=NULL){
			int j=0;
			for(;j<BUCKETSIZ;j++){	// makes check flag back to zero
				if(temp->komvoi[j]==NULL){
					continue;
				}
				temp->komvoi[j]->check=0;
			}
			temp=temp->next;
		}
	}
	return 0;
}

void resetHash(Hashinfo* hash){
	int hashsize=hash->size;
	int i=0;
	Bucket* start=NULL;
	for(;i<hashsize;i++){
		start=&(hash->table[i]);
		int j=0;
		Bucket* temp=start;
		while(temp!=NULL){
			for(;j<BUCKETSIZ;j++){
				if(temp->komvoi[j]==NULL){
					continue;
				}
				else{
					delete_Node(temp->komvoi[j]);
					temp->komvoi[j]=NULL;
				}
			}
			if(temp!=(&(hash->table[i]))){
				start=temp->next;
				free(temp);
				temp=start;
			}
			else{
				temp=temp->next;
				start->next=NULL;
			}
		}

	}
}

void printHash(Hashinfo* hash){
	int i=0;
	Bucket * temp=NULL;
	for(;i<hash->size;i++){
		temp=&hash->table[i];
		while(temp!=NULL){
			int j=0;
			for(;j<BUCKETSIZ;j++){	// makes check flag back to zero
				if(temp->komvoi[j]==NULL){
					continue;
				}
				print_data(stdout,temp->komvoi[j]);
			}
			temp=temp->next;
		}
	}
}

int createNode(Hashinfo* hashtable, int id){
	
	graphNode* temp=searchHash(hashtable,id);
	if(temp!=NULL){
		printf("failure:fail nodes %.7d already in hash\n",id);
		return 1;
	}
	temp=create_graphNode(id);
	if(temp==NULL){
		return -1;
	}

	int error=insertHash(hashtable,temp);
	if(error==1){
		fprintf(stdout, "failure:Failed inserting in Hash node %.7d\n",id );
		free(temp);
	}

	printf( "success: created %7.7d\n",id );
	return 0;
}

int deleteNode(Hashinfo* hashtable, int id){
	graphNode* temp=searchHash(hashtable,id);
	if(temp==NULL){
		fprintf(stdout, "failure:No node with id %7.7d\n",id);
		return 1;
	}
	if(temp->income!=0 || temp->outcome!=0){
		fprintf(stdout, "failure:Node %.7d has transactions with other nodes\n",id);
		return 1;
	}
	else{
		temp=removefromhash(hashtable,id);
		if(temp==NULL){
			printf("failure:removing from hash\n");
		}
		free(temp->payments);
		free(temp);
	}
	fprintf(stdout, "success: deleted %7.7d\n",id );
	return 0;
}


int addtran(Hashinfo* hashtable, int source, int target, double amount){
	graphNode* 	sender=searchHash(hashtable,source);
	graphNode*	receiver=searchHash(hashtable,target);
	if(sender==NULL){
		printf( "failure: Sender with id %.7d doesn't exist in hash...aborting\n",source);
		return 1;
	}
	if(receiver==NULL){
		printf( "failure: Receiver with id %.7d  doesn't exist in hash...aborting\n",target);
		return 2;
	}
	if(sender==receiver){
		printf("failure:Sender==Receiver\n");
		return 3;
	}
	if(add_akmh(sender,amount,receiver)){
		fprintf(stdout, "Couldn't create edge...aborting\n");
		return 4;
	}
	printf("success: added transaction %.7d  -> %.7d  with amount %.3f\n",source,target,amount);
	return 0;
}

int remtran(Hashinfo* hashtable, int source, int target){
	graphNode* 	sender=searchHash(hashtable,source);
	graphNode*	receiver=searchHash(hashtable,target);
	if(sender==NULL){
		printf("failure: Node with id %.7d  doesn't exist in hash...aborting\n",source);
		return 1;
	}
	if(receiver==NULL){
		printf("failure: Node with id %.7d  doesn't exist in hash...aborting\n",target);
		return 2;
	}
	if(remove_akmh(sender,receiver)){
		fprintf(stderr, "Couldn't create edge...aborting\n");
		return 3;
	}
	printf("success: removed transaction %.7d  -> %.7d \n",source,target);
	return 0;
}

double look_up(Hashinfo* hashtable,char* command, int target,int* error){
	graphNode* 	targetNode=searchHash(hashtable,target);
	double result;
	if(targetNode==NULL){
		printf("failure:Node with id %.7d  doesn't exist in hash...aborting\n",target);
		*error=1;
		return 1;
	}
	if(strcmp(command,SUM)==0){
		*error=0;
		result=lookup(targetNode,2);
		printf("success: SUM(%.7d )=%.3f\n",target,result);
		
	}
	if(strcmp(command,OUT)==0){
		*error=0;
		result=lookup(targetNode,1);
		printf("success: OUT(%.7d )=%.3f\n",target,result);
		 
	}
	if(strcmp(command,IN)==0){
		*error=0;
		result=lookup(targetNode,0);
		printf("success: IN(%.7d )=%.3f\n",target,result);
		
	}
	return result;
}

int triangle(Hashinfo* hashtable,int start,double minimum){
	graphNode* 	beginning=searchHash(hashtable,start);
	if(beginning==NULL){
		printf("failure:Node with id %.7d  doesn't exist in hash...aborting\n",start);
		return 1;
	}
	akmh* akmh1=beginning->payments->arxi;
	int holder[3];
	int flag=1;
	holder[0]=beginning->keyName;
	while(akmh1!=NULL){
		if(akmh1->poso<minimum){
			akmh1=akmh1->next;
			continue;
		}
		else{
			holder[1]=akmh1->nodeptr->keyName;
			akmh* akmh2=akmh1->nodeptr->payments->arxi;
			while(akmh2!=NULL){
				if(akmh2->poso<minimum){
					akmh2=akmh2->next;
					continue;
				}
				else{
					holder[2]=akmh2->nodeptr->keyName;
					akmh* akmh3=akmh2->nodeptr->payments->arxi;
					while(akmh3!=NULL){
						if(akmh3->nodeptr==beginning){
							if(akmh3->poso<minimum){
								break;		
							}	
							if(flag){
								printf("Success triangle (%.7d ,%f)=\n",holder[0],minimum);
								printf("(%.7d ,%.7d ,%.7d )\n",holder[0],holder[1],holder[2]);
								flag=0;
							}
							else{
								printf("(%.7d ,%.7d ,%.7d )\n",holder[0],holder[1],holder[2]);
							}
							break;
						}
						else{
							akmh3=akmh3->next;
							continue;
						}
					}
					akmh2=akmh2->next;
				}
			}
			akmh1=akmh1->next;
		}
	}
	if(flag){
		printf("No triangle triangle (%.7d ,%.3f)= 0\n",holder[0],minimum);
	}
	return 0;
}


int connection(Hashinfo* hashtable,int start,int end){
	graphNode* 	startNode=searchHash(hashtable,start);
	if(startNode==NULL){
		printf( "failure: Node with id %.7d  doesn't exist in hash...aborting Conn\n",start);
		return 1;
	}
	graphNode* 	endNode=searchHash(hashtable,end);
	if(endNode==NULL){
		printf("failure: Node with id %.7d  doesn't exist in hash...aborting Conn\n",end);
		return 1;
	}
	if(startNode==endNode){
		printf("failure:Same ids\n");
		return 1;
	}
	lista_akmwn temp;
	temp.arxi=NULL;
	temp.size=0;
	printf("Conn between node %d and node %d: \n",start,end);
	if(addakmh(&temp,startNode)!=0){
		return -1;
	}
	startNode->check=1;
	int result=pathfind(startNode,endNode,&temp);
	removeakmh(&temp,startNode);
	runthroughHash(hashtable);
	if(result==0){
		return 0;
	}
	else{
		printf("Conn(%.7d ,%.7d) not found \n",start,end);
		return 1;
	}
	
}


int allcycle(Hashinfo*hashtable,int start){		

	graphNode* 	startNode=searchHash(hashtable,start);		// vres sto hash ton komvo
	if(startNode==NULL){
		printf( "failure: Node with id %.7d  doesn't exist in hash...aborting allcycle\n",start);
		return 1;
	}
	lista_akmwn temp;
	temp.arxi=NULL;
	temp.size=0;
	if(addakmh(&temp,startNode)!=0){
		return -1;
	}
	startNode->check=1;
	printf("allcycles on node %d: \n",start);
	if(printcycles(startNode,startNode,&temp)==0){
		printf("No circles found involving %.7d\n",start);
	}	

	removeakmh(&temp,startNode);
	runthroughHash(hashtable);
	return 0;	
}
int traceflow(Hashinfo*hashtable,int start,int length){
	graphNode* 	startNode=searchHash(hashtable,start);			// vres sto hash ton komvo
	if(startNode==NULL){
		printf( "failure:Node with id %.7d  doesn't exist in hash...aborting allcycle\n",start);
		return 1;
	}
	lista_akmwn temp;		// ftia3e lista gia thn emfanish apotelesmatwn
	temp.arxi=NULL;
	temp.size=0;
	if(addakmh(&temp,startNode)!=0){	
		return -1;
	}

	startNode->check=1;
	printf("Traceflow on node %d: \n",start);
	if(path(startNode,&temp,length,0)==0){
		printf("No path found involving %.7d with size %d\n",start,length);
	}

	removeakmh(&temp,startNode);
	startNode->check=0;

	return 0;
}

void dump(Hashinfo* hashtable,char* filename){
 	FILE* target=fopen(filename,"w+");
 	
 	int i=0;
 	if(target==NULL){
 		printf("failure:error with fopen\n");
 		return;
 	}
 	fprintf(target,"%s",CRE_NOD);
 
 	for(;i<hashtable->size;i++){			// createnodes commands
 		Bucket* temp=&(hashtable->table[i]);
 		while(temp!=NULL){
 			
 		 	int j=0;
 		 	for(;j<BUCKETSIZ;j++){
 		 	
 		 		if(temp->komvoi[j]==NULL){
 		 			continue;
 		 		}
 		 		else{

 		 			int id=temp->komvoi[j]->keyName;
 		 			fprintf(target," %d",id);
 		 			
 		 		}
 		 	}
 		 	temp=temp->next;
 		}
 	}
 	fprintf(target,"\n");
 	for(i=0;i<hashtable->size;i++){		// add tran commands
 		Bucket* temp=&(hashtable->table[i]);
 		while(temp!=NULL){
 		 	int j=0;
 		 	for(;j<BUCKETSIZ;j++){
 		 	
 		 		if(temp->komvoi[j]==NULL){
 		 			continue;
 		 		}
 		 		else{

 		 			int id1=temp->komvoi[j]->keyName;
 		 			akmh* akmes=temp->komvoi[j]->payments->arxi;
 		 			
 		 			while(akmes!=NULL){		// for every   transaction
 		 				int id2=akmes->nodeptr->keyName;
 		 				double amount=akmes->poso;	
 		 				fprintf(target,"%s %d %d %f\n",ADD_TR,id1,id2,amount); 			
 	 					akmes=akmes->next;
 		 			}	
 	 			}
 	 		}
 	 		temp=temp->next;
 	 	}
 	}
 	fclose(target);
}