#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<malloc.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include    <sys/times.h>  /* times() */
#include	<unistd.h>
#include 	<string.h>
#include 	<ctype.h>
#include 	<fcntl.h>
#include 	<errno.h>
#include 	"Pointer_List.h"

#define 	MERGER		"./merger"
#define  	SORTER		"./sorter"


int main(int argc, char *argv[]){
	
	double t1, t2, cpu_time;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);

	int 	i=0;
	int*	readvote;
	int*	readcent;
	int*	readtime;
	char*	new_args[12];
	int 	start	=	atoi(argv[3]);		//arxh diavasmatos
	int 	end 	=	atoi(argv[4]);		// telos
	int 	id 		=	atoi(argv[6]);		// poio paidi apo ta l 
	int 	depth 	= 	atoi(argv[9]);		// se poio va8os vriskomaste
	int 	child	=	atoi(argv[10]);		// posa paidia na ftia3ei
	//int 	parentid=	atoi(argv[7]);	
	//pipe gia to time?
	char	childpipe[512];	//to pipe twn paidiwn gia tis pshfous twn upopshfiwn	
	char 	childcent[512]; // -//-				  gia ta eklogika kentra
	char 	childtime[512];// -//-				  gia tous xronous
	pid_t* cpid	=malloc(sizeof(pid_t)*child);
	readvote	=malloc(sizeof(int)*child);
	readcent	=malloc(sizeof(int)*child);
	readtime    =malloc(sizeof(int)*child);
	if(cpid==NULL||readvote==NULL||readcent==NULL||readtime==NULL){
		printf("Error with allocation of memory\n");
		return -1;
	}

	/*arguments: 0: program, 1 input, 2 pipe canditates, 5  pipe center,
	 		3 start, 4 end , 6 id, 7 parent id, 8 timepipe 9 depth 10 child 11 NULL
	*/
	
	new_args[1]=argv[1];
	
	//new_args[8]=timepipe
	//new_args[6]= id
	//new_args[7]=NULL;
	new_args[3]=malloc(sizeof(char)*16); //sizeof(int)/sizeof(char)
	new_args[4]=malloc(sizeof(char)*16);
	new_args[6]=malloc(sizeof(char)*16);
	if(new_args[3]==NULL||new_args[4]==NULL||new_args[6]==NULL){
		printf("Error with allocation of memory\n");
		return -1;
	}
	new_args[7]=argv[7];

	int size=end-start;
	int portion=size/child;
	if(portion==0){
		printf("childern >> data\n");
		portion=1;
	}
	int upoloipo=size- portion*child;


	for(i=0;i<child;i++){
		// sto arg[3] apo pou tha 3ekinhsei na diavazei to paidi
		sprintf(new_args[3],"%d",start);

		if(i!=child-1){   	// pros8ese to kommati pou tha diavasei to paidi
			start+=(portion);
		}
		else{				// sto teleutaio paidi pros8ese to upoloipo
			start+=(portion+upoloipo);
		}

		// sto arg[4] mexri pou tha diavasei to paidi
		sprintf(new_args[4],"%d",start);
		// h tautothta tou paidiou
		sprintf(new_args[6],"%d",i);

		//to pipe twn paidiwn gia tis pshfous twn upopshfiwn
		sprintf(childpipe,"%s%d%d%d",PIPEVOTE,depth,id,i);
		if(mkfifo(childpipe,0666)==-1 && (errno != EEXIST)){
			perror("Error creating the named pipe");
			return -1;
		}
		readvote[i]= open(childpipe, O_RDONLY | O_NONBLOCK);
		new_args[2]=childpipe;

		//to pipe twn paidiwn gia tis pshfous twn eklogikwn kentrwn
		sprintf(childcent,"%s%d%d%d",PIPECENT,depth,id,i);
		if(mkfifo(childcent,0666)==-1 && (errno != EEXIST)){
			perror("Error creating the named pipe");
			return -1;
		}
		readcent[i]= open(childcent, O_RDONLY | O_NONBLOCK);	
		new_args[5]=childcent;
		//to pipe twn paidiwn gia to time
		sprintf(childtime,"%s%d%d%d",PIPETIME,depth,id,i);
		if(mkfifo(childtime,0666)==-1 && (errno != EEXIST)){
			perror("Error creating the named pipe");
			return -1;
		}
		readtime[i]= open(childtime, O_RDONLY | O_NONBLOCK);
		new_args[8]=childtime;

		switch(cpid[i]=fork()){
		
		case 0:
			
		   if(depth>1){
		   		new_args[9]=malloc(sizeof(char)*16);
		   		
				if(new_args[9]==NULL){
					printf("Error with allocation of memory\n");
					return -1;
				}
				sprintf(new_args[9],"%d",(depth-1));
				
		   		new_args[10]=argv[10];
		   		new_args[0]=MERGER;
				new_args[11]=NULL;
			   	if(execvp(new_args[0],new_args)<0){
					perror("error with exec:");
			   		exit(1);
				}
			}
			else{

				new_args[9]=NULL;
				new_args[0]=SORTER;
				if(execvp(new_args[0],new_args)<0){
					perror("error with exec:");
				}
			}
			break;
		case -1:
		   /* take care of possible fork failure */
		   perror("fork failure: ");
		   break;
		
		default:
			break;
		   
		}
		fflush(stdout);
	}
	

	info_deikti center	=LIST_dimiourgia();
	info_deikti vote	=LIST_dimiourgia();
    tally rec;
	
	for(i=0;i<child;i++)
	{
   		int status;		
        waitpid(cpid[i], &status, 0);
      

        while(read(readvote[i],&rec,sizeof(tally))>0){
    		insertList(&rec,vote);
    	}
    	

    	while(read(readcent[i],&rec,sizeof(tally))>0){
    		insertList(&rec,center);
    		
    	}
    
    	
       
    	close(readvote[i]);
		close(readcent[i]);
        
	}


	
	int mkfifo 	= open(argv[2], O_WRONLY);
	if(mkfifo>=0){	
		LIST_diadromi(vote,0,mkfifo);
		
	}
	else{
		perror("mkfifo of canditates: ");
	}
	close(mkfifo);
	mkfifo 	= open(argv[5], O_WRONLY);
	if(mkfifo>=0){	
		LIST_diadromi(center,0,mkfifo);
		
	}
	else{
		perror("mkfifo of centers: ");
	}
	close(mkfifo);
	
	free(new_args[4]);
    free(new_args[3]);
    free(new_args[6]);
    free(readvote);
    free(readcent);
    free(cpid);
	LIST_katastrofi(&vote);
    LIST_katastrofi(&center);


    t2 = (double) times(&tb2);
    cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) -(tb1.tms_utime + tb1.tms_stime));
    double cpu_t=cpu_time / ticspersec;
    
    double real_t=(t2 - t1) / ticspersec;
    temp_time time1;
    setTime(cpu_t,real_t,start,depth,&time1);
    mkfifo  = open(argv[8], O_WRONLY);
    if(write(mkfifo,&time1,sizeof(temp_time))<0){
                perror("write problem in time pipe:");
                return -2;
    }  
	for(i=0;i<child;i++){
	    while(read(readtime[i],&time1,sizeof(temp_time))>0){
	    	if(write(mkfifo,&time1,sizeof(temp_time))<0){
	    	    perror("write problem in time pipe:");
	    	    return -2;
	    	}   
	    }
	    close(readtime[i]);
	}
	free(readtime);
	return id;
	
};