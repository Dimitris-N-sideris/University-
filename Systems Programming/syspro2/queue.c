
#include "queue.h"


head* find_channel(head * node,int id){
	
	while(node!=NULL){

		if(node->channel_id==id){
			return node;
		}
		node=node->next;
	}
	return NULL;
}


int add_channel(head **node,int chan_id,char* chan_name){
 	if(*node==NULL){
 		*node=create_channel(chan_id,chan_name);
 		if(*node==NULL){
 			return MEMERROR;
 		}
 		return CREAT_SUCCESS;
 	}
 	head* temp=*node;
 	while(temp->next!=NULL){
 		if(temp->channel_id==chan_id || strcmp(temp->channel_name,chan_name)==0){
 		 	return DUPLICATE;
 		}
 		temp=temp->next;
 	}
 	if(temp->channel_id==chan_id ||(strcmp(temp->channel_name,chan_name)==0)){
 		 	return DUPLICATE;
 	}
 	temp->next=create_channel(chan_id,chan_name);
 	if(temp->next==NULL){
 		return MEMERROR;
 	}
 	return CREAT_SUCCESS;
}

int write_channels(head * node){

	head * temp_node=node;

	while(temp_node!=NULL){
		printf("channel is : %d  %s\n",temp_node->channel_id,temp_node->channel_name);
		write_mess(temp_node->first);
		temp_node=temp_node->next;
	}

	return 0;
}

int write_mess(message_list* target){
	
	while(target!=NULL){
		printf("message is : %d  %s\n",target->type,target->message);
		target=target->next;
	}
	return 0;
}

int add_mes(head * node,int id,int typ,char * mes){


	
	while(node!=NULL){

		if(node->channel_id==id){
			
			message_list* temp=create_message(typ, mes);
			if(temp==NULL){
				return MEMERROR;
			}
			if(node->first==NULL){
				node->last=temp;
				node->first=temp;
				return 0;
			}
			node->last->next=temp;
			node->last=temp;
			break;
		}
		node=node->next;
	}
	if(node==NULL){
		return CHANNNOTEXIST;
	}
	return 0;
}

head* create_channel(int chan_id,char* chan_name){
	head* temp=malloc(sizeof(head));
	if(temp==NULL){
		return NULL;
	}
	temp->channel_id=chan_id;
	strcpy(temp->channel_name,chan_name);
	temp->first=NULL;
	temp->last=NULL;
	temp->next=NULL;
	return temp;
}


message_list* create_message(int typ,char* mes){
	message_list* temp=malloc(sizeof(message_list));
	if(temp==NULL){
		return NULL;
	}
	temp->type=typ;
	strcpy(temp->message,mes);
	temp->next=NULL;
	return temp;
}

void write_channel(int filed,head* start){
	//char input[CHARLIMIT];
	int header=0;
	while(start!=NULL){
		header=strlen(start->channel_name)+1;
		//sprintf(input,"%d%d%s",start->channel_id,header,start->channel_name);
		write(filed,&(start->channel_id),sizeof(int));
		write(filed,&header,sizeof(int));
		write(filed,(start->channel_name),header);
		printf("%d %d %s",start->channel_id,header,start->channel_name);
		start=start->next;
	}
	header=ENDOFFILE;
	write(filed,&header,sizeof(int));
	
}

void write_message(int filed,head* start,int target,char *path){
	head* list=find_channel(start,target);
	int size;
	if(list==NULL){
		size=ENDOFFILE;
		write(filed,&size,sizeof(int)); // shmadi termatismou
		return ;
	}
	message_list* temp=list->first;
	message_list* previous=list->first;

	while(temp!=NULL){
		if(temp->type>=1){ // arxeio
			if(write_file(filed,path,temp->message,temp->type)!=0){
				printf("error writing file %s\n",temp->message);
			}		
		}
		else{
			size=TYPE_MESS;
			write(filed,&size,sizeof(int));
			size=strlen(temp->message)+1;
			write(filed,&size,sizeof(int));
			write(filed,temp->message,size);
		}
		// delete input
		previous=temp;
		temp=temp->next;
		free(previous);
	}
	list->first=NULL;
	list->last=NULL;

	size=ENDOFFILE;
	write(filed,&size,sizeof(int)); // shmadi termatismou

}

int write_file(int filed,char*path,char* filename,int sizeofFILE){

	int size=0;
	char directory[CHARLIMIT];
	sprintf(directory,"%s/%s",path,filename);
	int file_to_copy=open(directory, O_RDONLY);
	if(file_to_copy<0){
		return 1;
	}
	size=TYPE_FILE;
	write(filed,&size,sizeof(int));
	size=strlen(filename)+1;
	write(filed,&size,sizeof(int));
	write(filed,filename,size);
			
	size=sizeofFILE;
	char  input[CHARLIMIT*4];

	write(filed,&size,sizeof(int));
	int size_read=0,get_bytes=0;
	do{
		size-=size_read;
		get_bytes=smaller_is(CHARLIMIT*4,size);
		size_read=read(file_to_copy,input,get_bytes);
		write(filed,input,size_read);
		usleep(TIMEOUT);
	}while(size_read<size);
	close(file_to_copy);
	unlink(directory);
	return 0;
}


int read_pipe(int filed,char*path,head* start,char* server2post,int *write_board){
	int  	id,header=0;
	int 	bytes_read=read(filed,&header,sizeof(int));
	sleep(1);
	
	char input[CHARLIMIT];
	if(bytes_read<0){
		//perror("error with post file:");
		return -1;
	}
	if(bytes_read==0){
		return 0;
	}

	switch(header){
		case OPENFIFO:
			*write_board=open(server2post, O_WRONLY);
		   	if(*write_board<0){
		   		perror("Error opening the named pipe ");
				return ErrorOpen;
   			}
   			break;
   		case CLOSEFIFO:
			close(*write_board);
   			break;
		case LISTHEADER:
			// open pipe for write
			write_channel(*write_board,start);
			break;
		case WRITEHEADER:
			
			bytes_read=read(filed,&id,sizeof(int));
			if(id<0){
				print_info(WRITE_FAIL);
			}
			if(bytes_read<0){
				perror("error with post write::");
			}
			
			readmsg(filed,input);
			
			header=add_mes(start,id,-1,input);

			if(header==CHANNNOTEXIST){
				write(*write_board,&header,sizeof(int));
				break;
			}
			header=WRITE_SUCCESS;
			bytes_read=write(*write_board,&header,sizeof(int));
			if(bytes_read<0){
				perror("write answer for WRITE failed:");
			}
			
			break;
		case SENDHEADER:
			bytes_read=read(filed,&id,sizeof(int));
			if(id<0){
				print_info(WRITE_FAIL);
			}
			if(bytes_read<0){
			 	perror("error with post send::");
			}
			int sizeofFile=readfile(filed,input,path);
			if(sizeofFile<0){
				return -1;
			}
			header=add_mes(start,id,sizeofFile,input);
			
			if(header==CHANNNOTEXIST){
				write(*write_board,&header,sizeof(int));
				break;
			}
			
			header=SEND_SUCCESS;
			write(*write_board,&header,sizeof(int));
			break;
		default:
		 	usleep(TIMEOUT);
	}
	
	return 0;
}


int readmsg(int read_fd,char * result){

  	int bytes_read=0,sizeofinput=0;
	
  	bytes_read=read(read_fd,&sizeofinput,sizeof(int));	// diavase to mege8os tou message
	if(bytes_read<0){
		perror("error with size of msg:");
		return -1;
	}
	bytes_read=read(read_fd,result,sizeofinput);		// diavase to message
	if(bytes_read<0){
		perror("error with msg:");
		return -1;
	}
	int leftover=sizeofinput-bytes_read;
	int pos_write=bytes_read;

	while(pos_write<sizeofinput){						// diavase to upoloipo message
		bytes_read=read(read_fd,&result[pos_write],leftover);   
		if(bytes_read<0){
			perror("error:123");
			return -1;
		}	
		leftover-=bytes_read;
		pos_write+=bytes_read;
	}
	result[sizeofinput]='\0';
	return 0;
}


int readfile(int read_fd,char * filename,char* path){		// diavazei ena arxeio kai to antigrafei sto path
	int leftover,bytes_read=0,sizeofinput;					// an path==NULL to swsei sto sxetiko path

	if(readmsg(read_fd,filename)==-1){		// failed to get filename
		return -1;
	}
	int newfile=0;
	char new_path[CHARLIMIT];	
	if(path!=NULL){							//create the path to the file
		sprintf(new_path,"%s/%s",path,filename);
		filename=new_path;
	}
	
	do{		// ftia3e neo arxeio me to onoma 		
		errno=0;						
		newfile=open(filename,O_WRONLY | O_CREAT | O_EXCL,0666);
		if(errno==EEXIST){ 	//( an xreiastei alla3e to )
			char new_filename[CHARLIMIT];
			strcpy(new_filename,filename);
			char * temp;
			temp=strtok(new_filename,".");
			if(temp==NULL){
				strcat(filename,newName);
			}
			else{
				sprintf(filename,"%s%s",temp,newName);
				temp=strtok(NULL,".");
				while(temp!=NULL){			// oso vriskeis epi8emata me "." 3anavalta
					sprintf(filename,"%s.%s",filename,temp);
					temp=strtok(NULL,".");
				}
			}
		}
		else if(newfile<0){
			perror("Couldn't create copy of file\n");
			return -1;
		}
	}while(newfile<0);

	bytes_read=read(read_fd,&sizeofinput,sizeof(int));	// diavase to mege8os tou arxeiou	
	if(bytes_read<0){
		perror("error:3");
		return -1;
	}
	int sizeofFILE=sizeofinput;
	char buffer[CHARLIMIT*4];
	bytes_read=0;
	leftover=0;
	do{
		
		//usleep(TIMEOUT/10);	
		sizeofinput-=bytes_read;								// meiwse ta bytes pou menoun
		leftover=smaller_is(CHARLIMIT*4,sizeofinput);		// des an ta bytes pou menoun einai perissotera apo to buffer
		bytes_read=read(read_fd,buffer,leftover);			// diavase apo to palio arxeio
		if(bytes_read<0){
			perror("error:4");
			return -1;
		}
		write(newfile,buffer,bytes_read);						// grapse sto neo arxeio
	}while(bytes_read<sizeofinput);							// osa ta bytes pou diavasthkan einai ligotera apo ta sunolika

	close(newfile);
	return sizeofFILE;
}



int smaller_is(int sizeA,int sizeB){
	if(sizeA<sizeB){
		return sizeA;
	}
	else{
		return sizeB;
	}
}


void print_info(int answer){

	if(answer==POSTSUCCESS){
		printf("Server received message\n");
	}
	else if(answer==LISTANWSER){
		//print results
	}
	else if(answer==CREAT_SUCCESS){
		printf("Server created the channel\n");
	}
	else if(answer==DUPLICATE){
		printf("CHANNEL ALREADY EXISTS\n");
	}
	else if(answer==SHUTDOWN_SUCC){
		printf("Shutdown success \n");
	}
	else if(answer==SHUTDOWN_FAIL){
		printf("Shutdown fail \n");
	}
	else if(answer==CHANNNOTEXIST){
		printf("CHANNEL DOESN'T EXIT\n");
	}
	else if(answer==COMMANDNOTFOUND){
		printf("error with command\n");
	}
	else if(answer==LISTFAIL){
		printf("NO CHANNELS\n");
	}
	else if(answer==WRITE_SUCCESS){
		printf("Successful write\n");
	}
	else if(answer==SEND_SUCCESS){
		printf("Successful send\n");
	}
	else if(answer==WRITE_FAIL){
		printf("Couldn't write \n");
	}
	else if(answer==SEND_FAIL){
		printf("Couldn't receive file \n");
	}
	else if(answer==MEMERROR) {
		printf("memory error\n");
	}
	else{
		printf("failure to recognize answer of server =.%d.\n", answer);
	}
}

