#include    <stdio.h>


#include  "queue.h"

#define FILE1 "acb.txt"

int 	server(char* path,char*server2client, char*client2server);
int 	client(char* path, char*client2server,char*server2client);
void 	printf_results(int read_fd,char* path);
void 	create_read(int read_fd);

int main(int argc,char* argv[]){
	signal(SIGINT , SIG_IGN);
	char pipenameS2C[CHARLIMIT];
	char pipenameC2S[CHARLIMIT];
	char server_id[CHARLIMIT];
	pid_t serverpid=0;
	int server_found=0;
	DIR * o_dir;
	struct dirent* direntdir;

	if(argc<2){
		printf("Please insert path\n");
		return NotEnoughArgs;
	}
	int error =mkdir(argv[1],PERMS);	// prospathei na ftia3ei to directory 
	if(errno!=EEXIST && error!=0){
		perror("Couldn't create path :"); 
		return ErrorCrtdir;
	}

	sprintf(pipenameC2S,"%s/%s",argv[1],Client2Server);  // arxika elegxei an uparxoun ta pipes
	sprintf(pipenameS2C,"%s/%s",argv[1],Server2Client);  
	sprintf(server_id,"%s/%s",argv[1],ServerID);  
	int serverfd=open(server_id,O_RDONLY);
	if(serverfd<0){			// check if there is a running process with the server id
		server_found=0;	
	}
	else{
		server_found+=1;
		read(serverfd,&serverpid,sizeof(pid_t));
		sprintf(server_id,"%s%d",PROC,serverpid);
		o_dir=opendir(server_id);
		if(o_dir==NULL){
			printf("No server process found under proc please reset server\n");
			return 1;	
		}
	}

	o_dir=opendir(argv[1]);
	if(o_dir==NULL){
		perror("opendir failed:");
	}
	direntdir=readdir(o_dir);
	while(direntdir!=NULL){	// psa3e sto katalogo an uparxei pipe tou server 
			
		if(direntdir->d_type==DT_FIFO){
			
			if(strcmp(direntdir->d_name,Client2Server)==0){ 
				
				server_found+=1;
			}
			if(strcmp(direntdir->d_name,Server2Client)==0){ 
				server_found+=1;
			}					
		}
		direntdir=readdir(o_dir);
	}
	if(server_found==0){	// no pipe founds
		switch(serverpid=fork()){
        
        case 0:			// server case
           server(argv[1],pipenameS2C,pipenameC2S);
            break;
        case -1:
          	/* take care of possible fork failure */
           perror("fork failure: ");
           break; 
        default:		//client
        	serverfd=open(server_id,O_RDWR | O_CREAT,0666);
            if(serverfd<0){
            	perror("Couldn't create server pid file \n");
            	return 1;
            }          
            write(serverfd,&serverpid,sizeof(pid_t));
            read(serverfd,&serverpid,sizeof(pid_t));

            client(argv[1],pipenameC2S,pipenameS2C);	
            break;       
        }  
    }
    else if (server_found==3){	// all pipes founds
    	client(argv[1],pipenameC2S,pipenameS2C);
    }
    else if(server_found==2){
    	printf("pipes founds but no process under /proc with server_id %d",serverpid);
    }
    else{
    	printf("server_found variable has unknown value %d",server_found);
    }
	return 0;
}

int server(char* path,char*server2client, char*client2server){
	
	char server2post[CHARLIMIT];	// dinei data sto boardpost 
	char post2server[CHARLIMIT];	// pairnei entoles apo to boardpost
	
	int  read_board;
	int  read_client;
	int  write_client;
	int  write_board=-1;
	sprintf(server2client,"%s/%s",path,Server2Client);
	
	sprintf(client2server,"%s/%s",path,Client2Server);
	sprintf(post2server,"%s/%s",path,Board2Server);
	sprintf(server2post,"%s/%s",path,Server2Board);
	
	
	// create pipes to be used
    if(mkfifo(server2client,PIPEPERMS)==-1){
    	perror("Error creating the named pipe");
        return PipeError;
    }
    if(mkfifo(client2server,PIPEPERMS)==-1){
    	perror("Error creating the named pipe");
        return PipeError;
    }
    if(mkfifo(post2server,PIPEPERMS)==-1){
    	perror("Error creating the named pipe");
        return PipeError;
    }
    if(mkfifo(server2post,PIPEPERMS)==-1){
    	perror("Error creating the named pipe");
        return PipeError;
    }
   
    // open pipes 
  	read_client =open(client2server, O_RDONLY| O_NONBLOCK);  
  	if(read_client<0){
   		perror("Error opening the named pipe ");
   		return ErrorOpen;
   	}	
   	write_client=open(server2client, O_WRONLY);
   	if(write_client<0){
   		perror("Error opening the named pipe ");
   		return ErrorOpen;
   	}
   	read_board =open(post2server, O_RDONLY | O_NONBLOCK); 
    if(read_board<0){
   		perror("Error opening the named pipe ");
   		return ErrorOpen;
   	}
	
   	// ready to run 4ever
   	char command[CHARLIMIT];
   	head *start_node=NULL;

    while(1){
    	usleep(TIMEOUT);
    	char * splitted;

    	read_pipe(read_board,path,start_node,server2post,&write_board);

    	int amount_read=read(read_client,command,CHARLIMIT);
    	if(amount_read<0){
    		continue;
    	}
    	if(amount_read==0){
    		usleep(TIMEOUT);
    		continue;
    	}

    	splitted=strtok(command," ");
    	int target= atoi(splitted);

    	while(amount_read<target){		// read whole command
    		amount_read+=read(read_client,&(command[amount_read]),CHARLIMIT-amount_read);
    	}
    	splitted=strtok(NULL," ");
    	int code_command=atoi(splitted);
    	splitted=strtok(NULL,"");  // rest of command
    	strcpy(command,splitted);
    	switch(code_command){

    		case CREATEHEADER:	// createchannel
    			splitted=strtok(command," ");
    			int channel_id=atoi(splitted);
    			splitted=strtok(NULL," \n");
    			int result=add_channel(&start_node,channel_id,splitted);  //try to create and add the new channel
  				write(write_client,&result,sizeof(int));				// return result to client
  				write_channels(start_node);	//

    			break;
    		case GETMESSGEHEADER:	 // getmessages
    			splitted=strtok(command," \n");
    			if(splitted==NULL){
    				amount_read=ENDOFFILE;
    				write(write_client,&amount_read,sizeof(int));
    				continue;
    			}
    			int target_id=atoi(splitted);			// get the target id of the channel
    			write_message(write_client,start_node,target_id,path);	// feed the data to the channel
    			break;
    		case SHUTDOWNHEADER:  //shutdown
    			//amount_read=SHUTDOWN_SUCC;
    			close(write_board);
    			close(read_board);
    			close(write_client);
    			close(read_client);
    			printf("%s____%s",client2server,post2server);
    			if(unlink(client2server)<0){
    				perror("C2S pipe Couldn't be unlinked");
    				//amount_read=SHUTDOWN_FAIL;
    			}	
				if(unlink(post2server)<0){
    				perror("P2S pipe Couldn't be unlinked");
    				//amount_read=SHUTDOWN_FAIL;
    			}
				if(unlink(server2post)<0){
    				perror("S2P pipe Couldn't be unlinked");
    				//amount_read=SHUTDOWN_FAIL;
    			}
    			
    			if(unlink(server2client)<0){
    				perror("S2C pipe Couldn't be unlinked");
    				//amount_read=SHUTDOWN_FAIL;
    			}	
    			//write(write_board,,&amount_read,sizeof(int));
    			exit(0);
    			break;
    		default:
    			amount_read=COMMANDNOTFOUND;
    			write(write_client,&amount_read,sizeof(int));
    			
    			usleep(TIMEOUT);
    	}
    	
    	memset(command,0,CHARLIMIT);
  	
    }

    // if u receive signal unlink pipes
    return 0;
}

int client(char* path, char*client2server,char*server2client){
	

	int writefifo=-1,readfifo=-1;
	sprintf(server2client,"%s/%s",path,Server2Client);
	sprintf(client2server,"%s/%s",path,Client2Server);
	
	while(writefifo<0 || readfifo <0){		// prepare the pipes 
		usleep(TIMEOUT);

		if(writefifo<0){
			writefifo=open(client2server, O_WRONLY); 
		}	
		if(readfifo<0){
			readfifo=open(server2client, O_RDONLY);
		}
	}

	char command[100];
	
	while(1){
		char * splitted=NULL;
		printf("Insert Command for board client\n%s id name\n%s id\n%s\n%s\n",CREATECHANNEL,GETMESSAGES,SHUTDOWN,EXIT);
		if(fgets (command,100, stdin)==NULL){
			continue;
		}
		splitted=strtok(command," \n");
		int sizeofwrite=0;
		if(splitted==NULL){
			printf("Command is wrong try again!");
			continue;
		}
		if(strcmp(command,CREATECHANNEL)==0){
			
			splitted=strtok(NULL," ");
		
			if(splitted==NULL){
				printf("error with command %s\n",CREATECHANNEL);
				continue;
			}
			sizeofwrite=strlen(splitted);
			int id= atoi(splitted);
			splitted=strtok(NULL," ");
			if(splitted==NULL){
				printf("error with command %s\n",CREATECHANNEL);
				continue;
			}

			sizeofwrite+=strlen(splitted)+4;
			sprintf(command,"%d %d %d %s",sizeofwrite,CREATEHEADER,id,splitted); //++
			
			if(write(writefifo,command,strlen(command)+1)<0){
				perror("write error:");
			}
			create_read(readfifo);
			continue;
		}
		if(strcmp(command,GETMESSAGES)==0){
			
			splitted=strtok(NULL," ");
			if(splitted==NULL){
				printf("error with command %s\n",GETMESSAGES);
				continue;
			}
			int id= atoi(splitted);
			sizeofwrite=strlen(splitted)+3;
			printf("getting messages of channel with %d\n",id);
			sprintf(command,"%d %d %s",sizeofwrite,GETMESSGEHEADER,splitted);
			write(writefifo,command,strlen(command)+1);
			printf_results(readfifo, path);
			continue;
		}
		if(strcmp(command,EXIT)==0){
			
			close(writefifo);
			close(readfifo);
			return 1;
		}
		if(strcmp(command,SHUTDOWN)==0){
			
			sizeofwrite=SHUTDOWNHEADER;		
			write(writefifo,&sizeofwrite,sizeof(int));
			return 1;
		}
	}
}

void create_read(int read_fd){		// reads answer 
	int answer;
	int bytes_read=read(read_fd,&answer,sizeof(int));
	if(bytes_read<0){
		perror("Error with read in client");
	}
	print_info(answer);
}

void printf_results(int read_fd,char* path){			

	int  type=0;
	int bytes_read=read(read_fd,&type,sizeof(int));
	if(bytes_read<0){
		perror("error in client :");
		return;
	}

	while(type!=ENDOFFILE && bytes_read > 0){	// diavaze mexri na vreis to telos h na ginei kapoio error
		char input[CHARLIMIT];
		switch(type){	// analoga to tupo
	
			case TYPE_MESS:		// an einai munhma ektupwse to

				
				readmsg(read_fd,input);
				printf("Message: %s\n",input);
				break;		
			case TYPE_FILE:		// an einai arxeio 
				printf("FILE found \n");		
				readfile(read_fd,input,NULL);	
				break;

			default: 
				printf("error with server message\n");
		}
		bytes_read=read(read_fd,&type,sizeof(int));		
	}
}


