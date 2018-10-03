#include    <stdio.h>

#include  "queue.h"

#define NotEnoughArgs 	-1
#define ErrorOpendir 	-2

#define CHARLIMIT 1024
#define ServerID		"_Spid"
#define Client2Server 	"_C2S"
#define Server2Client 	"_S2C"

int prepfile(int writedesc,char * filename,int id);
int readresult(int filed);
int readChannels(int readdesc);

int main(int argc,char* argv[]){
	
	signal(SIGINT , SIG_IGN);	
	char server_id[CHARLIMIT];
	pid_t serverpid=0;
	DIR * o_dir;
	sprintf(server_id,"%s/%s",argv[1],ServerID);  	// create pathe to file with serverid
	int serverfd=open(server_id,O_RDONLY);
	if(serverfd<0){			// check if there is a running process with the server id
		printf("No available server\n");
	}
	else{
		read(serverfd,&serverpid,sizeof(pid_t));
		sprintf(server_id,"%s%d",PROC,serverpid);
		o_dir=opendir(server_id);
		if(o_dir==NULL){
			printf("No server process found under proc please reset server (erase pipes etc)\n");
			return 1;	
		}
	}
	char server2board[CHARLIMIT];	// pairnei  data apo to client
	char board2server[CHARLIMIT];	// dinei data/ commands sto server
	int writefifo=-1,readfifo=-1;
	if(argc<2){
		printf("Please insert path\n");
		return NotEnoughArgs;
	}
	int header=0;
	sprintf(server2board,"%s/%s",argv[1],Server2Board);
	sprintf(board2server,"%s/%s",argv[1],Board2Server);
	
	while(writefifo<0 || readfifo <0){
	
		if(writefifo<0){
				writefifo=open(board2server,O_WRONLY );
				if(writefifo<0){
					perror("pipe for write not found:");
					return PipeError;
				}
				header=OPENFIFO;
				if(write(writefifo,&header,sizeof(int))<0){
					perror("first write to fifo failed:");
				}
		}

		if(readfifo<0){
				readfifo=open(server2board, O_RDONLY); 
				if(readfifo<0){
					perror("pipe for read not found:");
					return PipeError;
				}
				
		}
		
		
	}
	

	char command[CHARLIMIT];

	while(1){
		char * splitted=NULL;
		char message[CHARLIMIT];
		printf("Insert Command for boardpost\n%s for all the available channel\n%s id filename \n%s id message \n%s\n",LIST,SEND,WRITE,EXIT);
		if(fgets (command,100, stdin)==NULL){
			continue;
		}
		printf("Command is %s",command);
		splitted=strtok(command," \n");
		int sizeofwrite=0;
		if(splitted==NULL){
			printf("Command is wrong try again!");
			continue;
		}
		if(strcmp(command,SEND)==0){			// send file 
		
			
			splitted=strtok(NULL," ");
			if(splitted==NULL){
				printf("error with command %s\n",LIST);
				continue;
			}
			int id= atoi(splitted);
			printf("%s command in channel %d...Plesase insert filename\n",SEND,id);

			if(fgets (message,CHARLIMIT, stdin)==NULL){			// read the filename
				continue;
			}

			prepfile(writefifo,message,id);							// send file 		
			readresult(readfifo);								//read answer

			continue;
		}
		if(strcmp(command,WRITE)==0){			// send messsage 
			
			splitted=strtok(NULL," ");
			if(splitted==NULL){
				printf("error with command %s\n",WRITE);
				continue;
			}
			int id= atoi(splitted);
			printf("%s command in channel %d...Plesase insert message\n",WRITE,id);
			if(fgets (message,CHARLIMIT, stdin)==NULL){		//read message
				continue;
			}
			char*splitted=strtok(message,"\n");
			if(splitted==NULL){
				continue;
			}
			sizeofwrite=strlen(splitted)+1;
			printf("Sending to server command-> channel %d message:%s\n",id,splitted);
			int header=WRITEHEADER;
			write(writefifo,&header,sizeof(int));
			write(writefifo,&id,sizeof(int));
			write(writefifo,&sizeofwrite,sizeof(int));
			write(writefifo,splitted,strlen(splitted)+1);
			readresult(readfifo);				//read answer
		
			continue;
		}
		if(strcmp(command,LIST)==0){
			
			int header=LISTHEADER;		// ask for channels
			if(write(writefifo,&header,sizeof(int))<0){
				perror("write error:");
			}
			
			readChannels(readfifo);					//read channels
			continue;
			
		}
		if(strcmp(command,EXIT)==0){			//stop 
			int header=CLOSEFIFO;
			if(write(writefifo,&header,sizeof(int))<0){
				perror("last write to fifo failed:");
			}
			close(writefifo);
			close(readfifo);
			return 0;
		}		
	}

	return 0;
}

int readresult(int filed){
	int header;
	int bytes_read=read(filed,&header,sizeof(int));
	if(bytes_read<0){
		perror("Error with read in post");
	}
	print_info(header);
	return 0;
}


int readChannels(int readdesc){
	int header=0;
	char input[CHARLIMIT];
	int bytes_read=read(readdesc,&header,sizeof(int));
	if(bytes_read<0){
		perror("read channel error:");
		return -1;
	}
	while(header!=ENDOFFILE){
		int size,id=header;
		bytes_read=read(readdesc,&size,sizeof(int));
		if(bytes_read<0){
			perror("read channel error while taking string size:");
			return -1;
		}
		bytes_read=0;
		int pos_write=0,leftover=size;
		do{
			size-=bytes_read;
			bytes_read=read(readdesc,&input[pos_write],leftover);
			if(bytes_read<0){
				perror("read channel error while taking string size:");
				return -1;
			}	
			leftover-=bytes_read;
			pos_write+=bytes_read;
		}while(bytes_read<size);
		printf("Channel with %d and name %s\n",id,input);
		int bytes_read=read(readdesc,&header,sizeof(int));
		if(bytes_read<0){
			perror("read channel error while getting next channel:");
			return -1;
		}
	}
	return 0;
}

int prepfile(int writedesc,char * filename,int id){

	char*splitted=strtok(filename,"\n");
	int file=open(splitted, O_RDONLY);
	if(file<0){
		perror("file error:");
		return 1;
	}

	struct stat info;
	if(fstat(file,&info)<0){
		perror("stat failed:");
	}


	int sizeofwrite=strlen(filename)+1;
	int header=SENDHEADER;
	write(writedesc,&header,sizeof(int));
	write(writedesc,&id,sizeof(int));
	write(writedesc,&sizeofwrite,sizeof(int));
	write(writedesc,filename,strlen(filename)+1);
			
	sizeofwrite=info.st_size;
	char  input[CHARLIMIT*4];

	if(write(writedesc,&sizeofwrite,sizeof(int))<0){
			perror("write error while writing file to server:");
	}

	int size_read=0,get_bytes=0;
	do{
		sizeofwrite-=size_read;
		get_bytes=smaller_is(CHARLIMIT*4,sizeofwrite);
		size_read=read(file,input,get_bytes);
		if(write(writedesc,input,size_read)<0){
			perror("write error while writing file to server:");
		}
		usleep(TIMEOUT);
	}while(size_read<sizeofwrite);
	close(file);

	return 0;
}