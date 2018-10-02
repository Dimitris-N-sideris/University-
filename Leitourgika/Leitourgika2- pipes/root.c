#include    <stdio.h>
#include    <stdlib.h>
#include    <malloc.h>
#include    <sys/types.h>
#include    <sys/wait.h>
#include    <sys/times.h>  /* times() */
#include    <unistd.h>
#include    <string.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <errno.h>
#include    <signal.h>
#include    "Pointer_List.h"
#include    "record.h"
#define     MERGER      "./merger"
#define     SORTER      "./sorter"
/*
#define     PIPEVOTE    "/home/dimitris/Desktop/DimitrisSideris-Proj2/myfi/myvote"  
#define     PIPECENT    "/home/dimitris/Desktop/DimitrisSideris-Proj2/myfi/mycenter"
#define     PIPETIME    "/home/dimitris/Desktop/DimitrisSideris-Proj2/myfi/mytime" */
#define     CHAR_LIMIT   512

void handler();
int signalCount=0;

int main(int argc,char* argv[]){

	FILE * input;
    /*double t1, t2, cpu_time;
    struct tms tb1, tb2;
    double ticspersec;
    ticspersec = (double) sysconf(_SC_CLK_TCK);
    t1 = (double) times(&tb1);*/
	int option=0;	
    int depth=-1;					//είναι το βάθος του δένδρου που θα δημιουργηθεί
    int numOfSMs=-1;				//ο αριθμός των κόμβων splitters/mergers που θα πρέπει να δημιουργηθούν από κάθε κόμβο
    int percentile=-1;			   //είναι το ποσοστό ψήφων που θα αποδώσουν τα top εκλογικά κέντρα
    char inputFile[CHAR_LIMIT];		// input File txt morfh
    char outputFile[CHAR_LIMIT];	// output File tou root
    char outputCenter[CHAR_LIMIT];  // output File gia ta centers
    char * new_args[12];
	if(argc<11){
		printf("Not enough Arguments\n");
		return -1;
	}

	// epe3ergasia orismatwn
    while ((option=getopt(argc, argv, "l:d:p:o:i:c:")) != -1){

            switch(option){
            case'i':
                strcpy(inputFile,optarg);
                break;
			case'o':
                strcpy(outputFile,optarg);
                break;
            case'l':
                numOfSMs=atoi(optarg);
                break;			           
			case'd':
                depth=atoi(optarg);
                break;
            case'p':
                percentile=atoi(optarg);
                break;
			case'c':
				strcpy(outputCenter,optarg);
                break;
            default:
                printf("Arguments not recognizable and it is ignored\n");
                break;
            }
    }

    // calculate size
    input = fopen (inputFile,"rb");
    if (input==NULL) {
        printf("Cannot open binary file\n");
        return 1;
    }
    int     parentid=   getpid(); 
    // check number of records
    fseek (input , 0 , SEEK_END);
    long lSize = ftell (input);
    rewind (input);
    int size= (int) lSize/sizeof(record);
    
    char    childpipe[512]; //to pipe twn paidiwn gia tis pshfous twn upopshfiwn    
    char    childcent[512]; // -//-               gia ta eklogika kentra
    char    timepipe[512];
    int *   readvote;
    int *   readcent;
    int *   readtime;
    pid_t* cpid =malloc(sizeof(pid_t)*numOfSMs);
    readvote    =malloc(sizeof(int)*numOfSMs);
    readcent    =malloc(sizeof(int)*numOfSMs);
    readtime    =malloc(sizeof(int)*numOfSMs);
    if(cpid==NULL||readvote==NULL||readcent==NULL||readtime ==NULL){
        printf("Error with allocation of memory\n");
        return -1;
    }

    /*arguments: 0: program, 1 input, 2 output canditates, 5  output center,
                    3 start, 4 end , 6 id, 7 depth, 8 child 9 NULL
    */
    
    new_args[1]=inputFile;
    
    
    new_args[3]=malloc(sizeof(char)*16); //sizeof(int)/sizeof(char)
    new_args[4]=malloc(sizeof(char)*16);
    new_args[6]=malloc(sizeof(char)*16);
    new_args[7]=malloc(sizeof(char)*16); 
    if(new_args[3]==NULL||new_args[4]==NULL||new_args[6]==NULL||new_args[7]==NULL){
        printf("Error with allocation of memory\n");
        return -1;
    }

     // h tautothta tou root
        sprintf(new_args[7],"%d",parentid);

    int i=0;
    int start=0;
    int portion=size/numOfSMs;
    if(portion==0){
        printf("childern >> data\n");
        portion=1;
    }
    int upoloipo=size- portion*numOfSMs;
 
   
    for(;i<numOfSMs;i++){
        // sto arg[3] apo pou tha 3ekinhsei na diavazei to paidi
        sprintf(new_args[3],"%d",start);

        if(i!=numOfSMs-1){     // pros8ese to kommati pou tha diavasei to paidi
            start+=(portion);
        }
        else{               // sto teleutaio paidi pros8ese to upoloipo
            start+=(portion+upoloipo);
        }

        // sto arg[4] mexri pou tha diavasei to paidi
        sprintf(new_args[4],"%d",start);

        // h tautothta tou paidiou
        sprintf(new_args[6],"%d",i);

       

        //to pipe twn paidiwn gia tis pshfous twn upopshfiwn
        sprintf(childpipe,"%s%d%d%d",PIPEVOTE,depth+1,0,i);
        if(mkfifo(childpipe,0666)==-1 && (errno != EEXIST)){
            perror("Error creating the named pipe");
            return -1;
        }
        readvote[i]= open(childpipe, O_RDONLY | O_NONBLOCK);
        new_args[2]=childpipe;

        //to pipe twn paidiwn gia tis pshfous twn eklogikwn kentrwn
        
        sprintf(childcent,"%s%d%d%d",PIPECENT,depth+1,0,i);
        if(mkfifo(childcent,0666)==-1 && (errno != EEXIST)){
            perror("Error creating the named pipe");
            return -1;
        }
        readcent[i]= open(childcent, O_RDONLY | O_NONBLOCK);    
        new_args[5]=childcent;
        
        sprintf(timepipe,"%s%d%d%d",PIPETIME,depth+1,0,i);
        if(mkfifo(timepipe,0666)==-1 && (errno != EEXIST)){
            perror("Error creating the named pipe");
            return -1;
        }
        readtime[i]= open(timepipe, O_RDONLY | O_NONBLOCK);    
        new_args[8]=timepipe;
        
        //printf("MERGER args input %s readvote: %s readcent: %s end=%s start=%s  ,\n\n",new_args[1],new_args[2],new_args[5],new_args[3],new_args[4]);
        switch(cpid[i]=fork()){
        
        case 0:
            
           if(depth>1){
               // thesh gia to depth tou epomenou epipedou
                new_args[10]=malloc(sizeof(char)*16);  // thesh gia twn ari8mo
                new_args[9]=malloc(sizeof(char)*16);
                if(new_args[10]==NULL||new_args[9]==NULL){
                    printf("Error with allocation of memory\n");
                    return -1;
                }
                sprintf(new_args[9],"%d",(depth-1));
                sprintf(new_args[10],"%d",numOfSMs);
               
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
            signal(SIGUSR1,handler);
            break;
           
        }
        fflush(stdout);
    }
   
    info_deikti center  =LIST_dimiourgia();
    info_deikti vote    =LIST_dimiourgia();
    tally       rec;
    temp_time   tm1;

    FILE * stream_=fopen("time.txt","w");
    if(stream_==NULL)      perror("txt for time: ");

    for(i=0;i<numOfSMs;i++)
    {
        

        int status;

        waitpid(cpid[i], &status, 0);
         
         while(read(readvote[i],&rec,sizeof(tally))>0){
            rootInsert(&rec,vote);
        }
        sprintf(childpipe,"%s%d%d%d",PIPEVOTE,depth+1,0,i);

        while(read(readcent[i],&rec,sizeof(tally))>0){
            rootInsert(&rec,center);
             
  
        }
        sprintf(childcent,"%s%d%d%d",PIPETIME,depth+1,0,i);
        
        while(read(readtime[i],&tm1,sizeof(temp_time))>0){
           printTime(stream_,&tm1,size,depth,numOfSMs);
          
        }
      
        close(readvote[i]);
        close(readcent[i]);
        close(readtime[i]);
        if(unlink(childpipe)==-1){
            perror("Error unlinking canditates:");
        }
        if(unlink(childcent)==-1){
            perror("Error unlinking centers:");
        }
     
    }
    fclose(stream_);
    int maxSignals=1;
    for(i=0;i<depth;i++){
        maxSignals*=numOfSMs;
    }

    if(maxSignals==signalCount){
        printf("Caught a SIGUSR1 signal for each Sorter sum of signal:%d\n",signalCount);
    }
    else{
        printf("Caught %d SIGUSR1 signals should have caught %d\n",signalCount,maxSignals);   
    }

  
    
    stream_  = fopen(outputFile,"w");
    if(stream_!=NULL){  
        LIST_diadromif(vote,1,stream_);
        
    }
    else{
        perror("txt of canditates: ");
    }
    fclose(stream_);
    stream_  = fopen(outputCenter,"w");
    if(stream_!=NULL){     
        
        LIST_percent(center,percentile,stream_);
    }
    else{
        perror("txt of centers: ");
    }
    fclose(stream_);

    free(new_args[4]);
    free(new_args[3]);
    free(new_args[6]);
    free(new_args[7]);
    free(readvote);
    free(readcent);
    free(cpid);
    free(readtime);
    LIST_katastrofi(&vote);
    LIST_katastrofi(&center);



	return 0;
}


void handler()
{  signal(SIGUSR1,handler);
   signalCount+=1;
}