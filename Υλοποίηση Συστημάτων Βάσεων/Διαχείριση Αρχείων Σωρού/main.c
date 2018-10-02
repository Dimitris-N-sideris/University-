#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"

#define FILEBLOCK "TESTING"
#define	FILEREAD  "100.csv"




void Record_read(int FileDesc){

	FILE *readsrc;
	
	char input_line[sizeof(Record)+1];
	readsrc= fopen (FILEREAD,"rb");
	
	if(readsrc==NULL){
		fprintf(stderr,"error with input file\n");
		return ;
	}
	    
    int count=0;
   
    while(fgets(input_line,sizeof(Record)+1,readsrc)!=NULL){
		
		char * temp;
		int i=1;
		Record record;
		temp=strtok(input_line,",");
		while(temp!=NULL && i!=5){
				int adapt=0;
				switch(i){
					case(1):
						record.id=atoi(temp);
						break;
					case(2):
						adapt=strlen(temp);
						temp[adapt-2]='\0';
						strcpy(record.name,&temp[1]);
						break;
					case(3):
						adapt=strlen(temp);
						temp[adapt-2]='\0';
						strcpy(record.surname,&temp[1]);
						break;
					case(4):
						adapt=strlen(temp);
						temp[adapt-3]='\0';
						strcpy(record.city,&temp[1]);
						break;
					default:
						fprintf(stderr,"break");
				}
			temp=strtok(NULL,",");	
			i++;
		}
		if(HP_InsertEntry(FileDesc,&record)>=0) count ++;
		else fprintf(stderr,"NOT INSERTED\n");
	}

   	fprintf(stderr,"Inserted %d\n",count);
    fclose (readsrc);
}


int main() {
	//Add your test code here
	int		FileDesc;
	char	type[10];
	int		value=-1;
	char	char_value[20];

	if(HP_CreateFile(FILEBLOCK)<0){
		return -1;
	}
	
	if((FileDesc=HP_OpenFile(FILEBLOCK))<0){
		return -1;
	}
	
	Record_read(FileDesc);

  	//HOW U WANT TO PRINT??
	printf("Insert type of printing/search \nValid choices:~id~ ~name~ ~surname~ ~city~ ~all~\n");	
	while(value<0){	
		
		scanf("%s",type);
		if(strcmp(type,"id")==0){
			printf("Insert search target value\n");	
			scanf("%d",&value);
			HP_GetAllEntries(FileDesc,type,&value);
			value=0;
		}  
		else if(strcmp(type,"all")==0){
			HP_GetAllEntries(FileDesc,type,NULL);
			value=0;
		}
		else if(strcmp(type,"name")==0||strcmp(type,"surname")==0||strcmp(type,"city")==0){
			printf("Insert search target value\n");	
			scanf("%s",char_value);
			HP_GetAllEntries(FileDesc,type,char_value) ;
			value=0;
		}
		else printf("ERROR insert type again!!\nInsert type of printing/search \nValid choices:~id~ ~name~ ~surname~ ~city~ ~all~\n");	
	}

	
	
	if(HP_CloseFile(FileDesc)==-1){
		return -1;
	}
	return 0;
}

