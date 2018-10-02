
#include "record.h"
#define inputsad "testtest.txt"



// metaferei ena record pou diavasthke se tally gia upopshfious kai kentra antistoixa
void set_record(record*source,tally*target,int mode){
	if(mode){

		strcpy(target->name,source->name);
		target->quantity=1;
	}
	else{
		sprintf(target->name, "%hd",source->electionCenter);
		//printf("%s",target->name);
		target->quantity=1;
	}
}


// antigrafei ena tally se ena allo
void set_data(tally* target,const tally* source){
	if(source==NULL){
		//target->name[0]=0;
		memset(target->name, 0, 24);
		target->quantity=0;
	}
	else{
		strcpy(target->name,source->name);
		target->quantity=source->quantity;
	}
}

// emfanizei ena tally sto stream
int print_instream(FILE * stream,const tally* data){
	if(data_keno(data)==0){
		 //fflush(stream);
		fprintf(stream,"%s		%d \n",data->name, data->quantity);
		//fflush(stream);
		return 0;
	}
	else return -1;
}

int print_inwrite(int stream,const tally* data){
	
	if(data_keno(data)==0){
			if(write(stream,data,sizeof(tally))<0){
				perror("write problem in stream:");
				return -2;
			}	
		return 0;
	}
	else return -1;
}

//sugkrinei 2 onomata kai epistrefei 1 an einai idia.......alliws 0
int compare_name( const tally*  data1, const tally* data2){
		return (strcmp(data1->name,data2->name)==0);
}


int comparison( const tally*  data1, const tally* data2){
		return (strcmp(data1->name,data2->name));
}

int 	compare_quantity(const tally*  data1, const tally* data2){

	return data1->quantity<data2->quantity;
}


//epistrefei to kleidi
char* dataKleidi(tally*  target){
		return target->name;
}
//
int  add_quantity(tally*  target,const tally*  source){
		if(compare_name( target, source)){
				target->quantity+= source->quantity;
				return	target->quantity;
		}
		return 0;
}

// epistrefei to quantity
int 	 get_quantity(const tally* target){
		return target->quantity;
}
// au3anei to quantity
void 	increase_quantity( tally* target){
		target->quantity++;
}
//elegxei an to tally einai keno
int 	data_keno(const tally* target){
	return (strlen(target->name)<=1); 
}


void setTime(double cput_t,double real_t,int start,int depth,temp_time *temp){
	temp->realtime=real_t;
	temp->cputime=cput_t;
	sprintf(temp->name,"%d_%d",depth,start);
}

void	printTime(FILE * stream,temp_time *temp,int size,int max_depth, int numOfSMs ){
	char* command=strtok(temp->name,"_");
	int depth=atoi(command);
	command=strtok(NULL,"_");
	int start=atoi(command);
	int i=0;
	int portion=0;
	int initial=0;
	int pos=0;
	depth= max_depth-depth;

    for(;i<depth;i++){
        if(i!=0){
            portion = portion/numOfSMs;
  		}
        else{ 
            portion = size/numOfSMs;
        }
        if(portion==0){
        	portion=1;		
    	}
    	if(i!=0){
              initial+=numOfSMs*initial;
  		}        
    }
    if(portion==0){
    	printf("probably too many SMs for the data\n");
        portion=1;		
    }
    pos+=(size/portion);
    pos-=(size-start)/portion;
	if(depth==max_depth){
		fprintf(stream, "For sorter:%d   Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n",
		pos,temp->realtime,temp->cputime);
	}
	else{
		fprintf(stream, "For merger:%d   Run time was %lf sec (REAL time) although we used the CPU for %lf sec (CPU time).\n",
		pos+initial-1,temp->realtime,temp->cputime);
	}

}
