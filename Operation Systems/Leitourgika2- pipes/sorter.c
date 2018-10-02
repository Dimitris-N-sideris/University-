#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<malloc.h>
#include	<sys/types.h>
#include	<unistd.h>
#include 	<string.h>
#include 	<ctype.h>
#include 	<fcntl.h>
#include    <signal.h>
#include    <sys/times.h>  /* times() */
#include 	"record.h"
#include 	"LinearHash.h"

#define CHAR_LIMIT 250
#define LINE_LIMIT 500
#define WHITE "-"
int main(int argc, char * argv[]){
	
   
	double t1, t2, cpu_time;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);


    FILE * input;
    int i,start,end,output,pid;
	infohash* vote;
	infohash* center;
	Hash_creation(&center,5,0.76);
	Hash_creation(&vote,5,0.76);
    pid    =atoi(argv[7]);
	start  =atoi(argv[3]);
    end    =atoi(argv[4]);
    
	input 	= fopen(argv[1],"r");
    if (input==NULL) {
        printf("Cannot open binary file\n");
        return 1;
    }
    
    tally temp;
    record rec;
    //phgaine sto shmeio pou tha diavaseis to stream 
    fseek (input , sizeof(record)*start , SEEK_SET); 
  
    for (i=start; i<end ; i++) {
        
        if(fread(&rec, sizeof(record), 1, input)<0){
          perror("fread ");
        }
        // ta leuka 3exwrista
        if(rec.valid=='0'){
          strcpy(rec.name,"INVALID");
        }
        if(strcmp(rec.name,WHITE )==0){
            strcpy(rec.name,"LEUKO");
        }
        // hash gia votes ana upopshfio
    	 	set_record(&rec,&temp,1);
    	 	insert_hash(vote,&temp);
        // hash gia votes ana kentro  
    	 	set_record(&rec,&temp,0);
    	 	insert_hash(center,&temp);
    }
    
    fclose(input);

    // open pipes for write

    output 	= open(argv[2], O_WRONLY);
    // put into pipes
    printsorted(vote,output); 
    //printInfoHash(vote,output); 
    // kleisimo pipe
    close(output);    

    // open pipe for write
    output  = open(argv[5], O_WRONLY);
    // put into pipe
  	printsorted(center,output); 
    // kleisimo pipe
    

    close(output);

    // free hashes
    katastrofhHashtable(&vote);
    katastrofhHashtable(&center);
    kill(pid,SIGUSR1);

    t2 = (double) times(&tb2);
    cpu_time = (double) ((tb2.tms_utime + tb2.tms_stime) -(tb1.tms_utime + tb1.tms_stime));
   
	double cpu_t=cpu_time / ticspersec;

    double real_t=(t2 - t1) / ticspersec;
    temp_time tm1;
    setTime(cpu_t,real_t,start,0,&tm1);


    output  = open(argv[8], O_WRONLY);
    if(write(output,&tm1,sizeof(temp_time))<0){
        perror("write problem in time pipe:");
        return -2;
    }   
    return 1;
}
