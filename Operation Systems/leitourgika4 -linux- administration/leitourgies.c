
#include <stdio.h>
#include "leitourgies.h"


int startCNT(char * fileDirectory){
	int i,result[8];
	for(i=0;i<8;i++){
		result[i]=0;
	}
	cntFunction(fileDirectory,result);
	if(result[7]==0){
		printf("\nEroro   %d\n",result[7]);
	}
	for(i=0;i<7;i++){
		switch(i){
			case 0:
				printf("number of regular files: 	  %d   %f%\n",result[0], (double)result[0]/result[7]);
				break;
			case 1:
				printf("number of directories:  	  %d   %f%\n",result[1], (double)result[1]/result[7]);
				break;
			case 2:
				printf("number of symbolic links: 	  %d   %f%\n",result[6], (double)result[6]/result[7]);
				break;
			case 3:
				printf("number of named pipes:  	  %d   %f%\n",result[2], (double)result[2]/result[7]);
				break;
			case 4:
				printf("number of char devices:  	  %d   %f%\n",result[4], (double)result[4]/result[7]);
				break;
			case 5:
				printf("number of sockets:      	  %d   %f%\n",result[3], (double)result[3]/result[7]);
				break;
			case 6:
				printf("number of block device files:	  %d   %f%\n",result[5], (double)result[5]/result[7]);
				break;
			default:
				printf("impossibru :O\n");
		}
	}
	return 0;
}

int cntFunction(char * directory,int * result){
	
	printf("%s\n",directory);


	DIR * o_dir;
	struct dirent* data;
	char * temp =NULL;
	o_dir=opendir(directory);

	if(o_dir==NULL){
		printf("error with %s directory\n",directory);
		return -1;
	}

	while((data=readdir(o_dir))!=NULL){
		
		//printf("Hi\n");
		//printf("%uc   %s\n",data->d_type ,   data->d_name);
		switch(data->d_type){
			case DT_REG:	// regular files
				result[0]++;
				result[7]++;
				break;
			case DT_DIR:	// directories 

				if(strcmp(data->d_name,".")!=0&&strcmp(data->d_name,"..")!=0){ 
					result[1]++;
				    result[7]++;	
					if(crtPath(&temp,directory,data->d_name)==-1){// creates the path to this directory
						return -1;
					}  
					if(cntFunction(temp,result)==-1){// do it again for the every directory
						return -1;
					} 				
				}
				break;
			case DT_FIFO:	// named pipes
				result[2]++;
				result[7]++;
				break;
			case DT_SOCK:	// sockets
				result[3]++;
				result[7]++;
				break;
			case DT_CHR:	// character device
				result[4]++;
				result[7]++;
				break;
			case DT_BLK:	// block device
				result[5]++;
				result[7]++;
				break;
			case DT_LNK:	// symbolic link
				result[6]++;
				result[7]++;
				break;
			default:
				printf("DT_UNKNOWN\n");
		}

	}
	

	closedir(o_dir);
	if(temp!=NULL){
		free(temp);
		temp=NULL;
	}
	return 0;
}

 // apo tis shmeiwseis tou k. Delh
int crtPath(char** target,char * path, char * next){
	
	if(*target!=NULL){
		free(*target);
	}

	*target= (char*) malloc(strlen(path)+strlen(next)+2);
	if(target==NULL){
		return -1;
	}
	strcpy(*target,path);
	strcat(*target,"/");
	strcat(*target,next);
	//printf("new string %s  \n",*target);
	return 0;
}

int same_tty(int tty_id, char *directory){
	
	char * temp =NULL;
	char * splitted=NULL;
	char data[CHAR_LIMIT];
	
	if(crtPath(&temp,directory,STATUS)==-1){
		printf("error with path creation\n");
		return 0;
	} 	
	
	int input=open(temp,O_RDONLY);
	if(input<0){
		perror("Process data open error :");
		return 0;
	}

	if(read(input,data,CHAR_LIMIT)<0){
		perror("Process data read error :");
		return -1;
	}
	int i;
	
	int ppid=0;
	splitted=strtok(data," ");
	for(i=1;i<5;i++){
		if(splitted==NULL){
			printf("NULL\n");
			break;
		}
		switch(i){
			case 1:			// PID
				//printf("1  %d\n",(id=atoi(splitted)));
				break;		
			case 4:			//ppid
				ppid=atoi(splitted);
				//printf("4  %d\n",ppid);
				if(ppid==tty_id){
					return 1;
				}
				else{
					close(input);
					if(temp!=NULL){
						free(temp);
					}
					if(ppid==1 || ppid ==0){
						
						return 0;
					}
					else{
						sprintf(data,"%s/%d",PROC,ppid);
						return same_tty(tty_id,data);
					}
				}
				break;
		}
		splitted=strtok(NULL," ");
	}

	close(input);
	if(temp!=NULL){
		free(temp);
	}
	return 0;
}



int process_data(char * directory,unsigned long* kb_used, double* cpu_time){
	//DIR * o_dir;
	//struct dirent* data;

	char * temp =NULL;
	char * splitted=NULL;
	char data[CHAR_LIMIT];
	long long int 	starttime=0;
	double 			total_secs=0;
	char *output[9];
	if(crtPath(&temp,directory,STATUS)==-1){
		return -1;
	} 	
	
	int input=open(temp,O_RDONLY);
	if(input<0){
		perror("Process data open error :");
		return -1;
	}

	if(read(input,data,CHAR_LIMIT)<0){
		perror("Process data read error :");
		return -1;
	}
	int i;
	unsigned long  	unsigned_temp=0;
	long 			long_temp=0;
	
	splitted=strtok(data," ");
	for(i=1;i<25;i++){
		if(splitted==NULL){
			printf("NULL\n");
			break;
		}
		switch(i){
			case 1:			// PID
				output[0]=splitted;
				break;	
			case 3:			//STATE
				output[3]=splitted;
				break;
			case 4:			//PPID
				output[1]=splitted;
				break;
			case 14:    	// time user mode
				unsigned_temp+= strtoul(splitted,NULL,10);
				break;
			case 15:	 	// time kernel mode	
				unsigned_temp+= strtoul(splitted,NULL,10);
				break;
			case 16:		//childern waited time	(user mode)
				long_temp+= 	strtol(splitted,NULL,10);
				break;
			case 17:		//childern waited time	(kernel mode)
				long_temp+= 	strtol(splitted,NULL,10);
				if(long_temp<0){
					total_secs=(double)CLOCKS_PER*unsigned_temp/sysconf(_SC_CLK_TCK);
				}
				else{
					total_secs=(double)CLOCKS_PER*(unsigned_temp+long_temp)/sysconf(_SC_CLK_TCK);
				}
				
				*cpu_time+=total_secs;
				break;
			case 18:		// priority
				output[7]=splitted;
				//printf(":::::%s\n\n",splitted);
				break;
			case 22:		//start time 
				output[4]=splitted;
				break;
			case 23:		// virtual mem in bytes	
				
				unsigned_temp= strtoul(splitted,NULL,10)/1024;	// Kilobytes
				(*kb_used)+=unsigned_temp;
				break;
			case 24:		// rss (number of pages in real mem)
				long_temp=	 	(strtol(splitted,NULL,10)*sysconf(_SC_PAGESIZE))/1024; // Kilobytes
				break;
			default:
				//printf("nothing..%d\n",i);
				;
		}
		splitted=strtok(NULL," ");
	}
	for(i=0;i<=7;i++){
		if(i==2){
			printf("%4.0f 	",total_secs);
			continue;
		}
		if(i==4){
			starttime=strtol(output[i],NULL,10);
			starttime%=(60*60*24);
			printf("%lld:",starttime/(60*60));
			printf("%lld 	",(starttime%(60*60))/60);
			continue;
		}
		if(i==5){
			printf("%lu 	",unsigned_temp );
			continue;
		}
		
		
		if(i==6){
			printf("%ld 	", long_temp);
			continue;
		}
		printf("%s 	",output[i]);

	}
	
	close(input);
	if(crtPath(&temp,directory,CMDLINE)==-1){
		return -1;
	} 	
	input=open(temp,O_RDONLY);
	if(input<0){
		perror("Process data open error :");
		return -1;
	}
	memset(data,0,CHAR_LIMIT);
	int error;
	
	if((error=read(input,data,CHAR_LIMIT))<0){
		perror("Error with read in ps:");
		return -1;
	}
	i=0;
	while(error){
		if(data[i]==0){
			data[i]=' ';
		}
		i++;
		error--;
	}
	printf("	%s\n",data);
	close(input);
	if(temp!=NULL){
		free(temp);
	}
	return 0;
}



int user_processes(){
	DIR * o_dir;
	struct dirent*  data;
	struct stat 	info;
	char * temp =NULL;
	//char [15]
	o_dir=opendir(PROC);
	if(o_dir==NULL){
		printf("error with %s directory\n",PROC);
		return -1;
	}

	unsigned long 	total_mem_used=0;
	double 			total_cpu_time=0; 

	int user_id=getuid();
	int tty_id=getppid();
	
	printf("PID	PPID	CPU 	STATE	START 	VSZ 	RSS 	PRIORITY 	CMDLINE\n");
	while((data=readdir(o_dir))!=NULL){
			
		if(data->d_type==DT_DIR){
			
			if(strcmp(data->d_name,".")!=0&&strcmp(data->d_name,"..")!=0){ 
				
				if(crtPath(&temp,PROC,data->d_name)==-1){
						return -1;
				} 

				if(stat(temp,&info)<0){
					perror("error with stat: ");
				}
				
				// If owner = our user
				if(info.st_uid==user_id){// CHANGE TO ONLY TERMINAL
					//printf("process_data used\n\n");
					
					if(same_tty(tty_id,temp)==0){
						continue;
					}
					if(process_data(temp,&total_mem_used,&total_cpu_time)<0){
						printf("\n");
					//	printf("process_data <0\n");
					} 
				}
			}				
		}

	}
	printf("Total memory used %lu \n",total_mem_used);
	printf("Total cpu time %f \n",total_cpu_time);
	closedir(o_dir);

	if(temp!=NULL){
		free(temp);
	}
	return 0;
}

int parse_iostat(char* directory,process_info* temp_data){
	
	char*			temp=NULL;
	char*			splitted=NULL;
	char 			data[CHAR_LIMIT];
	
	if(crtPath(&temp,directory,FILE5)==-1){
		return -1;
	}
	int input=open(temp,O_RDONLY);

	if(input<0){
		//perror("Process data open error :");
		return -1;
	}
	
	if(read(input,data,CHAR_LIMIT)<0){
		perror("Process data read error :");
		return -1;
	}
	
	int i=0;
	splitted=strtok(data," \n");

	for(;i<6;i++){
		
		splitted=strtok(NULL," \n");
	
		temp_data->data[i+1]=atoi(splitted)/1024;

		splitted=strtok(NULL," \n");
	}
	return 0;
}


int iostat(char * mode, int numProcesses){
	info_deikti linfo=LIST_dimiourgia();
	int type=-1;
	if(strcmp(mode,PID)==0){
		type=0;
	}
	if(strcmp(mode,RCHAR)==0){
		type=1;
	}
	if(strcmp(mode,WCHAR)==0){
		type=2;
	}
	if(strcmp(mode,SYSR)==0){
		type=3;
	}
	if(strcmp(mode,SYSW)==0){
		type=4;
	}
	if(strcmp(mode,RFS)==0){
		type=5;
	}
	if(strcmp(mode,WFS)==0){
		type=6;
	}

	DIR * o_dir;
	struct dirent*  data;
	struct stat 	info;
	process_info 	temp_data;
	char * temp =NULL;
	o_dir=opendir(PROC);
	if(o_dir==NULL){
		printf("error with %s directory\n",PROC);
		return -1;
	}


	int user_id=getuid();
	
	printf("PID 	RCHAR 	WCHAR 	SYSR 	SYSW 	RFS 	WFS\n");
	while((data=readdir(o_dir))!=NULL){

		if(data->d_type==DT_DIR){
			//printf("Hi\n");
			if(strcmp(data->d_name,".")!=0&&strcmp(data->d_name,"..")!=0){ 
				
				// creates the path to this directory
				if(crtPath(&temp,PROC,data->d_name)==-1){
						return -1;
				} 
				
				temp_data.data[0]=atoi(data->d_name);
				if(stat(temp,&info)<0){
					perror("error with stat: ");
				}

				// If owner = our user
				if(info.st_uid==user_id){		// CHANGE TO ONLY TERMINAL
					//printf("parse_iostat \n\n");
					
					
					// get data from this directory
					if(parse_iostat(temp,&temp_data)<0){
						//printf("process_iostat <0\n");
						continue;
					}
					if(eisagwgh(linfo,&temp_data,type)<0){
						return -1;
					} 
				}
			}				
		}
	}

	closedir(o_dir);
	diadromh(linfo,numProcesses);
	if(temp!=NULL){
		free(temp);
	}
	printf("Destroy\n");
	LIST_katastrofi(&linfo);
	return 0;
}