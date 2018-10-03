#include 	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include  	<unistd.h>
#include 	<sys/socket.h>
#include 	<sys/types.h>
#include	<arpa/inet.h>
#include	<netinet/in.h>
#include	"domh.h"


int parse(FILE* readFrom, int socketPort);
int send_receive(int fileDesc, char* message,int max);


int main (int argc,char* argv[]){

	int port,option;
	char commandfile[CHARLIMIT];
	char hostNAME[CHARLIMIT];
	struct sockaddr_in server_address;
	struct sockaddr *serverptr=(struct sockaddr *)&server_address ;
    struct hostent *servername;
   
	if(argc<7){
		printf("Not Enough Arguments\n");
		return -1;
	}
	while ((option= getopt(argc,argv, "p:i:h:")) != -1){
		switch(option){
			case 'i':
				strcpy(commandfile,optarg);
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 'h':
				strcpy(hostNAME,optarg);
				break;
			default:
				printf("Invalid option ignored\n");
		}
	}
	sleep(1);
	printf("%s %d %s\n",commandfile,port,hostNAME);
	// open file
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("ERROR opening socket");
    }
    servername =  (struct hostent *)gethostbyname(hostNAME);
    if (servername == NULL) {
       printf("ERROR host not found\n");
       return 0;
    }
    server_address.sin_family = AF_INET;
    memcpy( (char *)&(server_address.sin_addr),(char *)servername->h_addr,servername->h_length);
    server_address.sin_port = htons(port);
   
    printf("Connecting to socket\n");
    if (connect(socketfd, serverptr,sizeof(server_address)) < 0){ 
            perror("ERROR connecting to socket");
            return 0;
    }
    
	FILE* commandFileds;
	if((commandFileds=fopen(commandfile,"r"))>0){  // parse command file		
		parse(commandFileds,socketfd);
		fclose(commandFileds);
	}
	else{
		printf("no command file found\n");
	}

	printf("Insert commands from keyboard\nEnter EXIT to exit\n");
	parse(stdin,socketfd);

	//ending free
	return 0;
}



int parse(FILE* readFrom, int socketPort){

	char command[CHARLIMIT];
	char message[CHARLIMIT];
	char * splitted;
	int length;
	while(fgets(command,CHARLIMIT,readFrom)!=NULL){
		memset(message,0,CHARLIMIT);
		splitted=strtok(command," \n");
		if(splitted==NULL){
			printf("please insert correct command\n");
			continue;
		}
		
		if(strcmp(splitted,MULT_TRANSFER)==0){
			length=0;
			//printf(message,"%s",MULT_TRANSFER);
			// wait for respone
			int amount;
			splitted=strtok(NULL," \n");
			if(isNum(splitted)==0){
				printf("error in command %s",MULT_TRANSFER);
				continue;
			}
			amount=atoi(splitted);
			sprintf(message,"%s %d",MULT_TRANSFER,amount);
			length+=strlen(message);
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				continue;
			}
			length+=sprintf(message+length," %s",splitted);
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				continue;
			}
			while(splitted!=NULL){

				length+=sprintf(message+length," %s",splitted);
				splitted=strtok(NULL," \n");
			}
			
			if(send_receive(socketPort,message,CHARLIMIT)<0){
				continue;
			}	
			printf("%s\n",message);
			continue;
		}
		if(strcmp(splitted,PRINT_MULT)==0){
			length=0;
			sprintf(message,"%s",PRINT_MULT);
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				continue;
			}
			length+=strlen(message);
			while(splitted!=NULL){

				length+=sprintf(message+length," %s",splitted);
				splitted=strtok(NULL," \n");				
			}

			if(send_receive(socketPort,message,CHARLIMIT)<0){
				continue;
			}	
			printf("%s\n",message);
			continue;
		}

		if(strcmp(splitted,ADD_ACCOUNT)==0){
		//	printf("%s\n",ADD_ACCOUNT);
			int initial_amount;
			splitted=strtok(NULL," \n");
			if(isNum(splitted)==0){
				continue;
			}
			initial_amount=atoi(splitted);
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				printf("error in command %s",ADD_ACCOUNT);
				continue;
			}
			sprintf(message,"%s %d %s",ADD_ACCOUNT,initial_amount,splitted);
			splitted=strtok(NULL," \n");
			if(splitted!=NULL){
				sprintf(message+strlen(message)," %s",splitted);
			}
			
			
			if(send_receive(socketPort,message,CHARLIMIT)){
				continue;
			}
			printf("%s\n",message);
			continue;
		}
		if(strcmp(splitted,PRINT_BAL)==0){
			//printf("%s\n",PRINT_BAL);
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				printf("error in command %s",PRINT_BAL);
				continue;
			}
			//commandsize=strlen(PRINT_BAL)+strlen(splitted)+3;
			sprintf(message,"%s %s",PRINT_BAL,splitted);
			splitted=strtok(NULL," \n");
			if(splitted!=NULL){
				sprintf(message+strlen(message)," %s",splitted);
			}
			
			if(send_receive(socketPort,message,CHARLIMIT)<0){
				continue;
			}	
			printf("%s\n",message);
			continue;
		}
		
		if(strcmp(splitted,TRANSFER)==0){
			//printf("%s\n",TRANSFER);
			int amount;
			char* source;
			splitted=strtok(NULL," \n");
			if(isNum(splitted)==0){
				printf("error in command %s",TRANSFER);
				continue;
			}
			amount=atoi(splitted);
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				printf("error in command %s",TRANSFER);
				continue;
			}
			source=splitted;
			splitted=strtok(NULL," \n");
			if(splitted==NULL){
				printf("error in command %s",TRANSFER);
				continue;
			}
			sprintf(message,"%s %d %s %s",TRANSFER,amount,source,splitted);
			splitted=strtok(NULL," \n");
			if(splitted!=NULL){
				sprintf(message+strlen(message)," %s",splitted);
			}
		
			if(send_receive(socketPort,message,CHARLIMIT)<0){
				continue;
			}	
			printf("%s\n",message);
			continue;
		}
		if(strcmp(splitted,EXIT)==0){
			//printf("%s\n",EXIT);

			
			sprintf(message,"%s",EXIT);
			if(send_receive(socketPort,message,CHARLIMIT)<0){
				continue;
			}	
			return 0;
		}
		if(strcmp(splitted,SLEEP)==0){
			printf("%s\n",SLEEP);
			splitted=strtok(NULL," \n");
			if(isNum(splitted)==0){
				printf("error in command %s",SLEEP);
				continue;
			}
			usleep(atoi(splitted));
		}
	}


	return 0;
}



int send_receive(int fileDesc, char* message,int max){
	int returned=0;
	int size=0;
	
	int commandsize=strlen(message);
	write(fileDesc,&commandsize,sizeof(int));
	do{
		returned=write(fileDesc,message+size,strlen(message)-size);
		if(returned<0){
			perror("Write error");
			return -1;
		}
		size+=returned;
	}while(size<commandsize);
	memset(message,0,max);
	commandsize=0;
	size=0;
	read(fileDesc,&commandsize,sizeof(int));
	//printf("abc\n\n");
	do{
		returned=read(fileDesc,message+size,commandsize-size);
		if(returned<0){
			perror("Read error");
			return -1;
		}
		size+=returned;
	}while(size<commandsize);
	//printf("22c\n\n");
	return 0;
}