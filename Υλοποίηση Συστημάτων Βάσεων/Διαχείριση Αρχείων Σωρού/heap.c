#include "BF.h"
#include "heap.h"


int max_cap= BLOCK_SIZE/ sizeof(Record);

int HP_CreateFile(char *fileName) {
  // Add your code here
	int 	error,fileDesc;
	void* 	block;

	BF_Init();

	if(error=BF_CreateFile(fileName)<0){   
		//BF_PrintError(error);
		fprintf(stderr,"can't create in %s \n",fileName);
		return -1;
	}
	fileDesc=BF_OpenFile(fileName);
	if(fileDesc<0){
		fprintf(stderr,"can't write in %s \n",fileName);
		return -1;
	}

	if(BF_AllocateBlock(fileDesc)<0){  
		fprintf(stderr,"can't allocate %s \n",fileName);
		return -1;
	}

	// INITIALIZATION OF FIRST BLOCK+

	if(BF_ReadBlock(fileDesc,0, &block)<0){
			fprintf(stderr,"can't read the file #%d \n",fileDesc);
			return -1;
	}
	((Record*)block)->id=-42;		// to prwto record tou prwtou block kanto 42

	if(BF_WriteBlock(fileDesc,0)<0){
		fprintf(stderr,"can't Initialize the file #%d \n",fileDesc);
		return -1;
	}
	//Ending the creation
	if(BF_CloseFile(fileDesc)<0){
		fprintf(stderr,"can't close %s \n",fileName);
		return -1;
	}
	return 0;
}

int HP_OpenFile(char *fileName) {
  // Add your code here
	int fileDesc;
	fileDesc=BF_OpenFile(fileName);

	if(fileDesc<0){
		fprintf(stderr,"can't write in %s \n",fileName);
		return -1;
	}

  	return fileDesc;
}

int HP_CloseFile(int fileDesc) {
  // Add your code here
	if(BF_CloseFile(fileDesc)<0){
		fprintf(stderr,"can't close the file #%d \n",fileDesc);
		return -1;
	}
  	return 0;
}

int HP_InsertEntry(int fileDesc, Record* record) {
	// Add your code here
	
	void *block;
	int i=0;
	int currentBlock=BF_GetBlockCounter(fileDesc)-1; 		// to teleutaio block


	if(BF_ReadBlock(fileDesc,currentBlock, &block)<0){		// fernei to teleutaio block
		fprintf(stderr,"can't read the file #%d \n",fileDesc);
		return -1;
	}

	
	Record* temp= (Record*)block;

	for(;i<max_cap;i++){			// int max_cap= BLOCK_SIZE/ sizeof(Record); xwrhtikothta enos clock se record

		if(temp[i].id==0){			// an uparxei kenh thesh vazei to Record
			
			memmove(&(temp[i]),(void*)record,sizeof(Record));		
			
			break;
		}
	}

	if(i==max_cap){					// an dn uphrxe 8esh sto block kanei allocate kai vazei sth prwth 8esh
		if(BF_AllocateBlock(fileDesc)<0){
			fprintf(stderr,"can't allocate block in the file #%d \n",fileDesc);
			return -1;
		}
		currentBlock++;
		if(BF_ReadBlock(fileDesc,currentBlock, &block)<0){
			fprintf(stderr,"can't read the file #%d \n",fileDesc);
			return -1;
		}
		//block[0]=record;
		memmove(block,(void*)record,sizeof(Record));

	}

	if(BF_WriteBlock(fileDesc,currentBlock)<0){			//grafei to block sto opoio evale to Record
		fprintf(stderr,"can't write the file #%d \n",fileDesc);
		return -1;
	}
	
	return 0;
}

void HP_GetAllEntries(int fileDesc, char* fieldName, void *value) {
  // Add your code here
	void *block;
	int i=0;
	int size=BF_GetBlockCounter(fileDesc);		//posa block uparxoun
	int blockcount=0;

	for(;i<size;i++){					// gia kathe block
		if(BF_ReadBlock(fileDesc,i, &block)<0){
			fprintf(stderr,"can't read the file #%d \n",fileDesc);
			return;
		}
		blockcount++;					// diavasthke +1
		Record*temp=(Record*)block;
		int j;
		for(j=0;j<max_cap;j++){			// gia kathe record pou xwraei se ena block
			
			if(temp[j].id==0) 	break;			// an einai 0 stamata to loop dn uparxoun alla
			if(temp[j].id==-42) 	continue;		// mn emfaniseis to keno record me to opoio arxikopoih8hke to file

				// analoga to input kai to value emfanise
			if(strcmp(fieldName,"all")==0){
			}
			else if(strcmp(fieldName,"id")==0){
				if(temp[j].id!= (*(int*)value)) continue;
			}
			else if (strcmp(fieldName,"name")==0){

				if(strcmp(temp[j].name,(char*)value)!=0) continue;
		
			}
			else if (strcmp(fieldName,"surname")==0){

				if(strcmp(temp[j].surname,(char*)value)!=0) continue;
			}
			else if (strcmp(fieldName,"city")==0)
			{
				if(strcmp(temp[j].city,(char*)value)!=0) continue;
			}
			else {
				fprintf(stderr,"Not a valid choice\n");
				return;
			}
			fprintf(stderr,"%d 	%s 	%s 	%s\n",temp[j].id,temp[j].name,temp[j].surname,temp[j].city);
		}
	}
	fprintf(stderr, "Number of blocks read: %d  \n",blockcount);
	
}
