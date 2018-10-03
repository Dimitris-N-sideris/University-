#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Hash.h"




int operationRead(Hashinfo* hashtable,FILE * input);


int main(int argc, char* argv[]){
	int 	option,hashsize=0;
	char 	input[CHARLIMIT];
	FILE* 	operations;
	while ((option=getopt(argc, argv, "o:b:")) != -1){

        switch(option){
            case'b':
                hashsize=atoi(optarg);
                break;
			case'o':
                strcpy(input,optarg);
                break;
            default:
                printf("Arguments not recognizable and it is ignored\n");
                break;
        }
    }
    if(hashsize==0){
    	fprintf(stderr, "Error with size of Hash table");
    	return 1;
    }
    Hashinfo* hashtable=hashCreation(hashsize);

    if(hashtable==NULL){
    	fprintf(stderr, "Error with creation of Hash table");
    	return 1;
    }

    operations=fopen(input,"r");
    if(operations!=NULL){
    	operationRead(hashtable,operations);
    }
    operationRead(hashtable,stdin);
    
    free(hashtable->table);	// for valgrind
    free(hashtable);
	
	return 0;
}

int operationRead(Hashinfo* hashtable,FILE * input){	
	char operation[CHARLIMIT];
	char *temp=NULL;
	printf("Insert command\n");
	while(fgets(operation,CHARLIMIT,input)!=NULL){
		//printf("operation read is %s-\n",operation);
		temp=strtok(operation," \n");
		if(temp==NULL){
			continue;
		}
		//printf("command is %s-\n",temp);
		if(strcmp(temp,CRE_NOD)==0){
			//printf("IN CRE_NOD\n");
			int count=0;
			int target=-1;
			do{
				temp=strtok(NULL," ");
				if(temp==NULL){
					if(count>0){
						continue;
					}
					printf("failure:Invalid input try %s id1...\n",CRE_NOD);
					continue;
				}
				target=atoi(temp);
				//printf("%d\n",target);
				if(createNode(hashtable,target)==0){
					count++;
				}
			}while(temp!=NULL);
			printf("%s command finished inserted %d nodes\n\n",CRE_NOD,count);
			continue;
		}
		if(strcmp(temp,DEL_NOD)==0){
			//printf("IN DEL_NOD\n");
			int count=0;
			int target=-1;
			do{
				temp=strtok(NULL," ");	
				if(temp==NULL){
					if(count>0){
						continue;
					}
					printf("failure:Invalid input try %s id1 ...\n",DEL_NOD);
					continue;
				}
				target=atoi(temp);
				if(deleteNode(hashtable,target)==0){
					count++;
				}
			}while(temp!=NULL);
			printf("%s command finished deleted %d nodes\n\n",DEL_NOD,count);
			continue;
		}
		if(strcmp(temp,ADD_TR)==0){
			//printf("IN ADD_TR\n");
			char* id1=strtok(NULL," ");	
			char *id2=strtok(NULL," ");		
			temp=strtok(NULL," ");
			if(temp==NULL||id2==NULL||id1==NULL){
				printf("failure:Invalid input try %s id1 id2 amount\n",ADD_TR);
				continue;
			}
			double amount=atof(temp);
			
			addtran(hashtable,atoi(id1),atoi(id2),amount);
			continue;
		}
		if(strcmp(temp,DEL_TR)==0){
			//printf("IN DEL_TR\n");
			char* id1=strtok(NULL," ");		
			temp=strtok(NULL," ");
			if(temp==NULL||id1==NULL){
				printf("failure:Invalid input try %s id1 id2\n",DEL_TR);
				continue;
			}
			remtran(hashtable,atoi(id1),atoi(temp));
			continue;
		}
		if(strcmp(temp,TRIAN)==0){
			//printf("IN TRIAN\n");
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id k\n",TRIAN);
				continue;
			}
			int start=atoi(temp);
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id k\n",TRIAN);
				continue;
			}
			double minimum=strtod(temp,NULL);
			
			triangle(hashtable,start,minimum);
		}
		if(strcmp(temp,LOOKUP)==0){
			//printf("IN LOOKUP\n");
			temp=strtok(NULL," ");
			char* id=strtok(NULL," ");
			if(temp==NULL|| id==NULL){
				printf("failure:Invalid input try %s SUM|OUT|IN id \n",LOOKUP);
				continue;
			}
			int target=atoi(id);
			int error=0;
			look_up(hashtable,temp,target,&error);
			if(error!=0){
				printf("failure:Invalid input try %s SUM|OUT|IN id \n",LOOKUP);
			}
			continue;
		}
		if(strcmp(temp,CONN)==0){
		//	printf("IN CONN\n");
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id1 id2\n",CONN);
				continue;
			}
			int start=atoi(temp);
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id1 id2\n",CONN);
				continue;
			}
			int end=atoi(temp);
			connection(hashtable,start,end);
			continue;
		}
		if(strcmp(temp,ALLCYCLES)==0){
			//printf("IN ALLCYCLES\n");
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id \n",ALLCYCLES);
				continue;
			}
			int target=atoi(temp);
			
			allcycle(hashtable,target);
			continue;
		}
		if(strcmp(temp,TRACE)==0){
			//printf("IN TRACE\n");
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id size\n",TRACE);
				continue;
			}
			int id=atoi(temp);
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s id  size\n",TRACE);
				continue;
			}
			int size=atoi(temp);
			
			traceflow(hashtable,id,size);
			continue;
		}
		if(strcmp(temp,BYE)==0){
			//printf("IN BYE\n");
			resetHash(hashtable);
			printf("success: cleaned memory\n");
		}
		if(strcmp(temp,PRINT)==0){
			//printf("IN PRINT\n");
			printHash(hashtable);
		}
		if(strcmp(temp,DUMP)==0){
			//printf("IN DUMP\n");
			temp=strtok(NULL," ");
			if(temp==NULL){
				printf("failure:Invalid input try %s filename\n",DUMP);
				continue;
			}
			dump(hashtable,temp);
		}
		printf("---------------------------------------\n");
	}
	return 0;
}