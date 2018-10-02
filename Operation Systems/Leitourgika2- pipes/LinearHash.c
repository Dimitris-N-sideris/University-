#include "LinearHash.h"

// Sunarthseis gia free
void katastrofhBucket(Bucket * to_del){
	if(to_del->next==NULL){
			free(to_del->bucket);
			return;
	}
	katastrofhBucket(to_del->next);
	free(to_del->bucket);
	free(to_del->next);
}	


void katastrofhHashtable(infohash ** hash){
	int i=(*hash)->round+1;
	int temp=ARXIKO;
	for(; i>0;i--){
		temp*=2;
	}
	for(;i<temp;i++){
			Bucket * temp1=(*hash)->table[i].arxikos;
			katastrofhBucket(temp1);
			free(temp1);
	}	
	free((*hash)->table);
	//fprintf(stderr," DESTROY\n");
	free(*hash);
	
}

// sunarthsh epiloghs kleidiou
int chooseFunction(infohash* hash,char* kwdikos){
	 int temp=getKey(hash,kwdikos,0);
	 
	 if(temp<hash->nextForSplit){
			temp=getKey(hash,kwdikos,1);
	}
	return temp;
}

// sunarthsh ulopoihsh ths sunarthshs tou kleidiou 
int getKey(infohash* hash, char* kwdikos,int mode){
	int i=hash->round+mode;
	int temp=ARXIKO;
	unsigned long int key=0;
	int size= strlen(kwdikos)*sizeof(char);
	if(size>=sizeof(unsigned long int)){
		size=sizeof(unsigned long int);
	}
	memcpy(&key,kwdikos,size);
	
	for(; i>0;i--){
		temp*=2;
	}
	//printf("s:%s	%lu 	 %d\n",kwdikos, key, key%temp);

	return key%temp;
}

char* getKwdikos(Bucket* target,int pos){
		
		return target->bucket[pos].key;
}


// psaxnei sta bucket gia th eggrafh me  id "kwdikos" kai thn epistrefei alliws NULL
tally* get(InfoBucket * target, int bucketsize, char* kwdikos){
		Bucket * temp= target->arxikos;
		while(temp!=NULL){
				int i=0;
				while(i<bucketsize){
						if(strcmp(getKwdikos(temp,i),kwdikos)==0){
								return getElement(temp,i);
							}
						i++;
				}
				temp=temp->next;
		}
	return NULL;
}

// elegxei an uparxei eggrafh me id "kwdikos" 
tally* check(infohash *hash,char* kwdikos){

	int key= chooseFunction(hash,kwdikos);
	return get(&hash->table[key],hash->bucketSize,kwdikos);

}


void setElement(Element* target,char* kleidi,tally* temp){
	
	
	strcpy(target->key,kleidi);
	set_data(&target->data,temp);

}

void increaseElement(Element* target){

	increase_quantity(&(target->data));

}


tally* getElement(Bucket * target, int pos){
	
	return  &target->bucket[pos].data;
}

int checkForSplit(infohash * hash){
	
	double temp=(hash->elements/(double)((hash->size)*(hash->bucketSize)));
	
	
	return temp>(hash->loadFactor);
	
}


// arxikopoiei ta bucket

int initializeBucket(Bucket * target,int num_buckets){
		int i=0;
		target->next=NULL;
		target->bucket=malloc(sizeof(Element)*num_buckets);
		if(target->bucket==NULL)
			return -1;
		
		for(;i<num_buckets;i++){
			setElement(&target->bucket[i],"",NULL);
		}
		return 0;
}
// dhmiourgei neo bucket

int  newBucket(InfoBucket * target,int num_buckets){
	
	
	Bucket ** temp=&target->arxikos;
	
	
	while(*temp!=NULL){
		temp=&((*temp)->next);
	}	
	*temp=malloc(sizeof(Bucket));
	if(temp==NULL) return -1;
	if(initializeBucket(*temp,num_buckets)==0){
			return 0;
	}	
	return -1;
}

// vazei eggrafh sta bucket
int putInBucket(int bucketsize,InfoBucket* target,tally* input){
	int i;
	int flag=1;
		
	Bucket * temp=target->arxikos;
	while(temp!=NULL){
		i=0;	
		
	
		while(i<bucketsize){
		
			if(strcmp(getKwdikos(temp,i),"")==0){

					target->number_of_elements++;
					
					setElement(&temp->bucket[i],dataKleidi(input),input);
					flag=0;
					break;
			}

			if(strcmp(getKwdikos(temp,i),dataKleidi(input))==0){
				flag=0;
				target->number_of_elements++;
				increaseElement(&temp->bucket[i]);
				break;
			}
			i++;
		}
		if(flag==0) break;
		temp=temp->next;	
	}
	if(flag) {	
		if(newBucket(target,bucketsize))   return 1;  
		putInBucket(bucketsize, target, input);
	}
	return 0;
}


int clean_bucket(Bucket * temp,int bucketsize){		// an kapoio overflow bucket dn xrhsimopoieitai meta to split to svhnei
	int i=0;
	
	if(temp!=NULL) i=clean_bucket(temp->next,bucketsize);
	
	else 	return 0;
	
	if(i==1) {
		free(temp->next->bucket);
		free(temp->next);
		temp->next=NULL;
	}
	
	i=0;	
	while(i<bucketsize){
			if(getElement(temp,i)!=NULL)  return 0;
			i++;
	}
	return 1;
}



// dhmiourgia kai arxikopoihsei ths domhs tou hash table
int Hash_creation(infohash** hash,int bucket_Size,float load_Factor){		
	int i=0;
	*hash=malloc(sizeof(infohash));
	if(hash==NULL) 	return -1;
	(*hash)->table=malloc(sizeof(InfoBucket)*ARXIKO*2);
	if((*hash)->table==NULL) 	return -2;
	for(; i<ARXIKO*2;i++){
		(*hash)->table[i].number_of_elements=0;
		(*hash)->table[i].arxikos=NULL;
		if(newBucket(&((*hash)->table[i]),bucket_Size)<0) return -3;	
	}
	(*hash)->nextForSplit=0;
	(*hash)->round =0;
	(*hash)->size=ARXIKO;
	(*hash)->elements=0;
	(*hash)->bucketSize=bucket_Size;
	(*hash)->loadFactor=load_Factor;	
	return 0;
}

// allazei to gurw kai kanei realloc to xwros gia ton epomeno guro
int increaseRound(infohash** hash){

	int temp=ARXIKO;
	int i=(*hash)->round+1;
	for(; i>0;i--){
			temp*=2;
	}
	
	if(temp==(*hash)->size){		// an to hash table exei ginei diplasiastei
					// alla3e guro
		(*hash)->round++;			
		InfoBucket *new_ptr=NULL;
		if((new_ptr=realloc((*hash)->table,temp*2*sizeof(InfoBucket)))==NULL) {	// kane realloc
				fprintf(stderr, "realloc problem\n");
				return -5;
		}
		(*hash)->table=new_ptr;
		
		
		i=temp;					// apo to telos tou palio pinaka 
		for(; i<temp*2;i++){	// mexri to neo telos tou pinaka pou egine realloc
								//Arxikopoiei ta adeia stoixeia
			(*hash)->table[i].number_of_elements=0;
			(*hash)->table[i].arxikos=NULL;
			if(newBucket(&((*hash)->table[i]),(*hash)->bucketSize)<0) {
				fprintf(stderr, "memory problem, error with initialization\n");
				return -3;	
			}
		}
		(*hash)->nextForSplit=0;
	}
	
	return 0;	
}


void split(infohash* hash){
	int pos=hash->nextForSplit;			
	
	Bucket * temp=hash->table[pos].arxikos;
	hash->nextForSplit++;
	
	while(temp!=NULL){
		int i=0;
		
		while(i<hash->bucketSize){
			tally target;
			tally* temp_data=NULL;
			temp_data=getElement(temp,i);
			if(temp_data!=NULL){					// an uparxei eggrafh  3anaperna th sto hash
				set_data(&target,temp_data);
				int key=chooseFunction(hash,dataKleidi(temp_data));
				hash->table[pos].number_of_elements--;
				setElement(&temp->bucket[i],"",NULL);
				putInBucket(hash->bucketSize,&(hash->table[key]),&target);
			}
			i++;
		}
		temp=temp->next;
	}
	
	clean_bucket(hash->table[pos].arxikos,hash->bucketSize);		// svhnei ta bucket overflow pou dn xrhsimopoiountai
	hash->size++;

}


int insert_hash(infohash* hash,tally* input){			// xrhsimopoiei tis alles sunarthseis gia na dhmiourghsei to linear hash table
	
	int pos=chooseFunction(hash,dataKleidi(input));
	//printf("%d   ",pos);
	if(putInBucket(hash->bucketSize,&(hash->table[pos]),input))	return -1;
	
	hash->elements++;
	
	if(checkForSplit(hash)){
		split(hash);
		increaseRound(&hash);
	}
	return 0;

}


tally* getdata(Element * target){
		return &(target->data);
}


void printBucket(InfoBucket* Info, int Bucket_Size,int output){
	Bucket * temp= Info->arxikos;
	
	while(temp!=NULL){
		int i=0;
		for(;i<Bucket_Size;i++){
				if(data_keno(getElement(temp,i))) break;
				//fprintf(stderr,"BUCKET PRINT pos :%d\n",i);
				print_inwrite(output,getElement(temp,i));
		}
		temp=temp->next;
	}
}


void printInfoHash(infohash* hash,int output){
		int i=0;
		for(;i<hash->size;i++){
			if(hash->table[i].arxikos==NULL) continue;
			printBucket(&hash->table[i],hash->bucketSize,output);
		}
	
}

tally* getMax(InfoBucket* Info, int Bucket_Size, tally* max){
	Bucket * temp= Info->arxikos;
	tally	* newmax;
	while(temp!=NULL){
		int i=0;
		for(;i<Bucket_Size;i++){
				newmax=getElement(temp,i);
				if(data_keno(newmax)) continue;

				if(comparison(max,newmax)<0){
					return newmax;
				}
				else{
					return max;
				}
		}
		temp=temp->next;
	}
	return max;
}


void printsorted(infohash* hash,int output){
	
	while(hash->elements){
		tally max;
		set_data(&max,NULL);
		tally *new_max=&max;
		int i=0;
		for(;i<hash->size;i++){
			if(hash->table[i].arxikos==NULL) continue;
			new_max=getMax(&hash->table[i],hash->bucketSize, new_max);			

			if(new_max!=&max){
				set_data(&max,new_max);
				//pos=i;
			}
		}
		print_inwrite(output,new_max);
		set_data(new_max,NULL);
		set_data(&max,NULL);

		fflush(stderr);
		hash->elements--;
	}

}
