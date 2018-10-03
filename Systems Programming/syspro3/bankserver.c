#include 	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"domh.h"


extern char* optarg;
void* 	worker(void* args);
int 	write_amount(int fileDesc,char* message);
int 	bind_on_port(int sock ,short port,struct sockaddr_in* server);

int main (int argc,char* argv[]){
	
	int option,queuesize=5,port=99,poolsize=20;
	int socketNum,i,error=0;
	char hostname[CHARLIMIT];
	struct sockaddr_in client_addr;
	struct sockaddr* clientptr=(struct sockaddr*)&client_addr;
	
	int fileDesc;
	pthread_t* mythreads;
	if(argc<7){
		printf("Not Enough Arguments\n");
		return -1;
	}
	// parse arguments
	while ((option= getopt(argc,argv, "p:s:q:")) != -1){
	 	switch(option){
	 		case 'p':		// port argument
	 			port=atoi(optarg);
	 			break;
	 
	 		case 's':		// thread pool size argument
	 			poolsize=atoi(optarg);
	 			break;
	 
	 		case 'q':		// thread queue size argument
	 			queuesize=atoi(optarg);
	 			break;
	 
	 		default:
	 			printf("Invalid option ignored\n");
	 	}
	}
	printf("p:%d,s:%d _ %d\n",port,queuesize,poolsize);
	// creates and prepares socket
	if((socketNum = socket(AF_INET ,SOCK_STREAM , 0))==-1){
		perror("Socket creation failed!");
	}
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	client_addr.sin_port = htons(port);
	if(bind(socketNum , clientptr ,sizeof(client_addr))<0){
		perror("Socket bind failed!");
	}
	
	if(listen(socketNum,queuesize)<0){
		perror("Socket listen failed!");
	}

	if(gethostname(hostname,CHARLIMIT)<0){
		perror("error with hostname:");
	}
	printf("%s\n",hostname);
	
	// create the main struct 
	Hashinfo* hashtable=createHash(HASHSIZE,LOCKS,queuesize);
	

	if(hashtable==NULL){
		printf("error with hash creation\n");
		return -1;
	}
	
	mythreads=malloc(sizeof(pthread_t)*poolsize);
	if(mythreads==NULL){
		printf("error with thread creation\n");
		return -1;
	}
	// create the pool of threads
	for(i=0;i<poolsize;i++){
		if((error=pthread_create(&mythreads[i],NULL,worker,(void*)hashtable))<0){
			perror("thread creation error:");
			exit(1);
		}
	}
	
	while(1){
		// take fileDesc 
		
		if((fileDesc = accept(socketNum , NULL , NULL))< 0) { // waiting for message in socket
			perror("accept error");
			continue;
		}
		// put fileDesc to queue for workers
		pushClient(hashtable,fileDesc);

	}


	return 0;
}




void* worker(void* args){ // arguments locks + hash + queue
	int sizeRead,leftover;
	char message[CHARLIMIT];			
	char command[CHARLIMIT];
	char*splitted;
	char *holder;
	char * sourcename;
	int error=0, delay;
	int amount;
	sunallages* nameholder=NULL;
	
	Hashinfo* hash=(Hashinfo*) args;		// pare to struct me tis plhrofories apo to orisma args
	int fileDesc;
 	

 	while(1){			// pairne Filedescriptor apo thn oura
 		
 		memset(message,0,CHARLIMIT);
 	
 		fileDesc=popClient(hash);
 		
 			while(1){			// diavaze sunexeia apo to File descriptor
 				
 				delete_(nameholder);	// adeiase thn 
 				nameholder=NULL;
 		
 				// diavase thn entolh
 				leftover=0;
 				error=read(fileDesc,&sizeRead,sizeof(int));
 				if(error<0){
 					perror("worker reading size of message error\n");
 					return NULL;
 				}
 				
 				memset(command,0,CHARLIMIT);
 				while(leftover<sizeRead){
 					error=read(fileDesc,&command[leftover],sizeRead-leftover);
 					if(error<0){
 						perror("worker reading  message error\n");
 						break;
 					}
 					leftover+=error;
 				}
 			
 				// parse command
 				splitted=strtok_r(command," \n",&holder);
 				if(splitted==NULL){
 					return NULL;
 				}
 				if(strcmp(splitted,ADD_ACCOUNT)==0){
 					
 					splitted=strtok_r(NULL," \n",&holder);
 					if(splitted==NULL){
 						return NULL;
 					}	
 					amount=atoi(splitted);
 					splitted=strtok_r(NULL," \n",&holder);
 					if(splitted==NULL){
 						return NULL;
 					}
 					sourcename=splitted;
 					splitted=strtok_r(NULL," ",&holder);
 					if(isNum(splitted)){
 						delay=atoi(splitted);
 					}
 					else{
 						delay=0;
 					}
 					
 					if(add_accountHash(hash,sourcename,amount,delay)>=0){
 						sprintf(message,"Account creation (%s:%d)[%d]",sourcename,amount,delay);
 					}
 					else{
 						sprintf(message,"Account creation failed (%s:%d)[%d]",sourcename,amount,delay);
 					}
 						//printf("%s\n",message);
 					write_amount(fileDesc,message);	
 					continue;		
 				}
 				if(strcmp(splitted,PRINT_BAL)==0){
 					sprintf(message,"Success. Balance  ");
 					splitted=strtok_r(NULL," ",&holder);
 					if(splitted==NULL){
 						return NULL;
 					}
 					addsunallagh(&nameholder,splitted,0,hash->hashsize);
 					sourcename=splitted;
 					splitted=strtok_r(NULL," ",&holder);
 					if(isNum(splitted)){
 						delay=atoi(splitted);
 					}
 					else{
 						delay=0;
 					}
 					if(getBalance(hash,nameholder,message,delay)==0){
 						sprintf(message+strlen(message)," [%d]\n",delay);
 					}
 					else{
 						//memset(message,0,CHARLIMIT);
 						sprintf(message,"Failed. Balance %s  [%d]\n",sourcename,delay);
 					}
 					write_amount(fileDesc,message);	
 					continue;
 				}
 				if(strcmp(splitted,PRINT_MULT)==0){
 					sprintf(message,"Multi-Balance ");
 					splitted=strtok_r(NULL," \n",&holder);
 					
 					sourcename=splitted;
 					if(sourcename==NULL){	
 						return NULL;
 					}
 					splitted=strtok_r(NULL," \n",&holder);

 					while(splitted!=NULL){
		
 						addsunallagh(&nameholder,sourcename,0,hash->hashsize);	// add the previous;
 						sourcename=splitted;						
 						splitted=strtok_r(NULL," \n",&holder);		// get next one
 					}
 					if(isNum(sourcename)){ // for the last one check if it is a number
 						delay=atoi(sourcename);
 					}
 					else{
 						addsunallagh(&nameholder,sourcename,0,hash->hashsize);
 						delay=0;
 					}
 					if(getBalance(hash,nameholder,message,delay)==0){
 						sprintf(message+strlen(message),"\n");
 					}
 					else{
 						//memset(message,0,CHARLIMIT);
 						sprintf(message,"Failed.Multi-Balance");
 						add_name(nameholder,message);
 					}
 					write_amount(fileDesc,message);	
 					continue;
 				}
 				if(strcmp(splitted,TRANSFER)==0){
 				//	sprintf(message,"%s addition",TRANSFER);
 					splitted=strtok_r(NULL," \n",&holder);
 					if(splitted==NULL){
 						return NULL; 
 					}
 					//printf("%")
 					amount=atoi(splitted);
 					splitted=strtok_r(NULL," \n",&holder);
 					if(splitted==NULL){
 						return NULL;
 					}
 					//printf("%d  %s.\n\n",amount,splitted);
 					addsunallagh(&nameholder,splitted,1,hash->hashsize);
 					sourcename=splitted;
 					splitted=strtok_r(NULL," \n",&holder);
 					if(splitted==NULL){
 						return NULL;
 					}
 					char* target=splitted;
 					addsunallagh(&nameholder,splitted,0,hash->hashsize);
 					int sizeofList=1;
 					splitted=strtok_r(NULL," ",&holder);
 					if(isNum(splitted)){
 						delay=atoi(splitted);
 					}
 					else{
 						delay=0;
 					}
 					memset(message,0,CHARLIMIT);
 					if(addTransfer(hash,nameholder,amount,sizeofList,delay)==0){
 						sprintf(message,"Transfer addition (%s:%s:%d)[%d]\n",sourcename,target,amount,delay);
 					}
 					else{
 						sprintf(message,"Transfer addition failed (%s:%s:%d)[%d]\n",sourcename,target,amount,delay);
 					}
 					write_amount(fileDesc,message);	
 					continue;
 				}
 				if(strcmp(splitted,MULT_TRANSFER)==0){
 				
 					splitted=strtok_r(NULL," \n",&holder);
 					
 					if(splitted==NULL){
 						return NULL;
 					}
 					amount=atoi(splitted);
 					splitted=strtok_r(NULL," \n",&holder);
 					if(splitted==NULL){
 						return NULL;
 					}
 					addsunallagh(&nameholder,splitted,1,hash->hashsize);
 					char* source=splitted;
 					splitted=strtok_r(NULL," \n",&holder);
 					int sizeofList=0;
 					
 					sourcename=splitted;
 					if(sourcename==NULL){	
 						return NULL;
 					}
 					splitted=strtok_r(NULL," \n",&holder);

 					while(splitted!=NULL){
		
 						addsunallagh(&nameholder,sourcename,0,hash->hashsize);	// add the previous;
 						sourcename=splitted;						
 						splitted=strtok_r(NULL," \n",&holder);		// get next one
 						sizeofList++;
 					}

 					if(isNum(sourcename)){ // for the last one check if it is a number
 						delay=atoi(sourcename);
 					}
 					else{
 						addsunallagh(&nameholder,sourcename,0,hash->hashsize);
 						sizeofList++;
 						delay=0;
 					}
 					/*
 					while(splitted!=NULL){
 						addsunallagh(&nameholder,splitted,0,hash->hashsize);
 						splitted=strtok_r(NULL," \n",&holder);
 					
 					}*/
 					memset(message,0,CHARLIMIT);
 					if(addTransfer(hash,nameholder,amount,sizeofList,delay)==0){
 						sprintf(message,"Multi-Transfer addition (%s:%d)[%d]\n",source,amount,delay);
 					}
 					else{
 						sprintf(message,"Multi-Transfer addition failed (%s:%d)[%d]\n",source,amount,delay);
 					}

 					write_amount(fileDesc,message);	
 					continue;
 				}
 				if(strcmp(splitted,EXIT)==0){
 					//memset(message,0,CHARLIMIT);
 					sprintf(message,"Bye Bye");
 					write_amount(fileDesc,message);	
 					break;
 				}
 				sprintf(message,"Command not Found\n");
 				write_amount(fileDesc,message);	
 			}
 		}
	// do work wait for other work
} 

int write_amount(int fileDesc,char* message){
	int amount=strlen(message)+1;
	int check=0;
	write(fileDesc,&amount,sizeof(int));
	int wrote=0;
	while(amount>wrote){
			check=write(fileDesc,message+wrote,amount-wrote);
			if(check<0){
				perror("Write error account add");
				return -1;
			}
			wrote+=check;
	}
	return 0;
}

int bind_on_port(int sock ,short port,struct sockaddr_in* server){

	server->sin_family = AF_INET;
	server->sin_addr.s_addr = htonl(INADDR_ANY);
	server->sin_port = htons(port);
	return bind(sock , (struct sockaddr *) server ,sizeof(server));
}

