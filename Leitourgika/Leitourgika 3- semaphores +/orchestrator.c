#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "Datastruct.h"

#define VOTER_EXE       "./voter"
#define COUNTER_EXE     "./counter"
#define CHECKER_EXE     "./checker"
#define CREATED_VOTERS  "appendonly.txt"
#define SHDMID_FLAG     "-s"
#define SLEEP_FLAG      "-d"
#define CHOICE_FLAG     "-c"
#define REGNUM_FLAG     "-r"
#define REGISTRY_FLAG   "-i"
#define OUTPUT_FLAG     "-o" 
#define MAX_AM          99999999
#define MIN_AM          10000000
int ALLsema_unlink();
int registry_order(voter* registry,int start,int end);

int main(int argc, char* argv[]){
    ALLsema_unlink();
	char   inputFile[CHARLIMIT];
    char   choiceFile[CHARLIMIT];
    char   outputFile[CHARLIMIT];
    int    counterperiod;            //time for votes
    int    checkerperiod;          // time for checkers
    int    seats;
    int    numofvoters;
    int    random;
    int    option;
    char * new_args[10];
    pid_t  cpid;
    pid_t* voter_ids;
    FILE * input;
    if(argc<15){
        printf("not enough Arguments\n");
        return ARG_ERROR;
    }

	while ((option=getopt(argc, argv, "i:n:d:t:c:o:l:")) != -1){

            switch(option){
            case'i':
                strcpy(inputFile,optarg);
                break;
			case'o':
                strcpy(outputFile,optarg);
                break;
            case'c':
                strcpy(choiceFile,optarg);
                break;
            case'l':
                seats=atoi(optarg);
                break;  
            case'n':
                numofvoters=atoi(optarg);
                break;
            case'd':
                counterperiod=atoi(optarg);
                break; 
            case't':
                checkerperiod=atoi(optarg);
                break;                			           
            default:
                printf("Arguments not recognizable and it is ignored\n");
                break;
            }
    }
    input   = fopen(inputFile,"r");
    if (input==NULL) {
        printf("Cannot open txt file\n");
        return 1;
    }
    
    int size=0;
    char inputline[CHARLIMIT];
    while(fgets(inputline,CHARLIMIT,input)){
        size++;
    }

    rewind(input);
    //CREATE SHARED MEMORY SEGMENTS
    int shmid = shmget(IPC_PRIVATE,sizeof(segment),0666);  //  segment used to share info between processes
    if(shmid==-1){
        perror("Segment 0 error:");
        return -1;
    }
    segment* temp=shmat(shmid,NULL,0);
    if((int)temp==-1){
        perror("segment attachment");
        return -1;
    }
    temp->registrysize=size;            //registry size
    temp->queuesize=seats;              //counter queue size 

    int arrayid = shmget(IPC_PRIVATE, size*sizeof(voter),0666);    //segment for registry used by checkers 
    if(arrayid==-1){
        perror("Segment 1 error:");
        return -1;
    }
    temp->arrayshmid[0]=arrayid;

    arrayid = shmget(IPC_PRIVATE, seats*sizeof(voter),0666);       //segment for counter to see voters
    if(arrayid==-1){
        perror("Segment 2 error:");
        return -1;
    }
    temp->arrayshmid[1]=arrayid;

    


    arrayid = shmget(IPC_PRIVATE, CHECKERS*sizeof(voter),0666);           // segment for checkers to see voters
    if(arrayid==-1){
        perror("Segment 3 error:");
        return -1;
    }
    temp->arrayshmid[2]=arrayid;
    int i;
    



    voter* registry     =shmat( temp->arrayshmid[0],NULL,0);
    voter* counter      =shmat( temp->arrayshmid[1],NULL,0);
    voter* checkertable =shmat( temp->arrayshmid[2],NULL,0);
   
    // ARXIKOPOIHSH PINAKWN
   // printf("INITIALIZING\n");

    for (i=0; i<size ; i++){

        if(fgets(inputline,CHARLIMIT,input)){
           //put in segment 
            registry[i].AM=atoi(strtok(inputline," "));
          //  printf("%d\n", registry[i].AM);
            registry[i].choice=-1;
            registry[i].voteright=0;
        }
        else{
            printf("stopped before size \n");
            break;
        }    
    }
    registry_order(registry,0,size-1);
    if(fgets(inputline,CHARLIMIT,input)){
        printf("FILE STILL NOT EMPTY \n");
    } 
    for(i=0;i<seats;i++){
        counter[i].AM=0;
        counter[i].choice=0;
        counter[i].voteright=0;
    }

    for(i=0;i<CHECKERS;i++){
        checkertable[i].AM=0;
        checkertable[i].choice=0;
        checkertable[i].voteright=0;
        temp->available_checker[i]=0;
    }
    // ta3inomhsh registry me to AM 
    fclose(input);

    // get how many options there are
    input   = fopen(choiceFile,"r");
    if (input==NULL) {
        printf("Cannot open binary file\n");
        return 1;
    }
    int choices=0;
    while(fgets(inputline,CHARLIMIT,input)){
        choices++;
    }
    fclose(input);
    temp->choices=choices;

    new_args[2]=malloc(sizeof(int)*16);     // shmid num
    new_args[4]=malloc(sizeof(int)*16);     // choice/ time period of sleep
    if(new_args[2]==NULL ||new_args[4]==NULL){
        printf("Error with memory allocation\n");
    }

    new_args[0]=COUNTER_EXE;
    new_args[1]=SHDMID_FLAG;
    sprintf(new_args[2],"%d",shmid);
    new_args[3]=SLEEP_FLAG;
    sprintf(new_args[4],"%d",counterperiod);
    new_args[5]=REGISTRY_FLAG;
    new_args[6]=inputline;
    new_args[7]=OUTPUT_FLAG;
    new_args[8]=outputFile;
    new_args[9]=NULL;


    switch(cpid=fork()){
        case 0:
            if(execvp(new_args[0],new_args)<0){
                perror("error with exec:");
            }
            break;
        case -1:
            perror("fork failure: ");
            break;
        default:
           // printf("counter created\n")
        ;
    }

    new_args[6]=malloc(sizeof(int)*16);     // choice/ time period of sleep
    if(new_args[6]==NULL){
        printf("Error with memory allocation\n");
    }

    // CREATE checkers 
     // common arguments for all checkers
    new_args[0]=CHECKER_EXE;
    sprintf(new_args[4],"%d",checkerperiod);
    new_args[5]=CHOICE_FLAG;
    new_args[7]=NULL;
    pid_t  checkerpid;
    for(i=0;i<CHECKERS;i++){
        sprintf(new_args[6],"%d",i);

        switch(checkerpid=fork()){
            case 0:
                if(execvp(new_args[0],new_args)<0){
                    perror("error with exec:");
                }
                break;
            case -1:
                perror("fork failure: ");
                break;
            default:
                //printf("checker created\n")
            ;
        }
    }
    // prepare voters
    srand(time(NULL));
    // common arguments for all voters
    new_args[0]=VOTER_EXE; 
    new_args[3]=REGNUM_FLAG;
    new_args[5]=CHOICE_FLAG;
    new_args[7]=NULL;
    int regnum;

    voter_ids=malloc(sizeof(pid_t)*numofvoters);
    if(voter_ids==NULL){
        printf("Error with memory allocation\n");
        return -1;
    }
    // empty append file
    input=fopen(CREATED_VOTERS,"w");
    fclose(input);
    input=fopen(CREATED_VOTERS,"a+");
    if(input==NULL){
        printf("Error with created voter file\n");
        return -1;
    }
    int countdown=0;
    int type;
    int division;
    printf("Voters are coming\n");
    for(i=0;i<numofvoters;i++){
        int choice=rand()%choices;
        countdown++;
      //  division=numofvoters/6;
        switch(rand()%6){  
            case 0:    // get from register number not used
            case 4:
            case 1:
            case 3:    
            case 2:   
                type=0;
                int flag;
                rewind(input);
                flag=0;
                regnum=registry[rand()%size].AM;
                type=0;
                fseek(input,0,SEEK_SET);   

                while(fgets(inputline,CHARLIMIT,input)){
                    if(atoi(strtok(inputline," "))==regnum){
                        type=1;
                        break;
                    }
                }
                break;
            case 5:   // random number until it's not in register...  
                do{
                    flag=0;
                    regnum=rand()%(MAX_AM-MIN_AM)+MIN_AM;
                    for(type=0;type<size;type++){
                        if(regnum==registry[type].AM){
                            flag=1;
                        }
                        else if(registry[type].AM>regnum){
                            break;
                        }
                    }
                }while(flag);
                type=2;
                break;
            default:
                printf("this shouldn't happen wtf\n");;
        }

        sprintf(new_args[4],"%d",regnum);     
        sprintf(new_args[6],"%d",choice);
       
        switch(voter_ids[i]=fork()){
            case 0:
                if(execvp(new_args[0],new_args)<0){
                    perror("error with exec:");
                }
                break;
            case -1:
                perror("fork failure: ");
                break;
            default:
               // printf("voter created\n");
                fprintf(input,"%d %d %d\n",regnum,choice,type);
                
        }
    }
    fclose(input);
   
    //wait voters to end
    int status;
    for ( i = 0; i < numofvoters; i++) {
        
        waitpid(voter_ids[i], &status, 0);
    }
    printf("No voters left ... ending\n");
    // send 3 voters to end each checker and count
    new_args[0]=VOTER_EXE;
    new_args[1]=SHDMID_FLAG;
    sprintf(new_args[2],"%d",shmid);
    new_args[3]=REGNUM_FLAG;
    sprintf(new_args[4],"%d",-1);
    new_args[5]=CHOICE_FLAG;
    sprintf(new_args[6],"%d",-1);
    new_args[7]=NULL;
    for(i=0;i<CHECKERS;i++){
        switch(voter_ids[i]=fork()){
            case 0:
                if(execvp(new_args[0],new_args)<0){
                    perror("error with exec:");
                }
                break;
            case -1:
                perror("fork failure: ");
                break;
            default:
               ;
        }
    }
    // wait for counter to end 
    waitpid(cpid, &status, 0);
   
    // put data in output txt

    input=fopen(outputFile,"a");
    int mult_count=0;   // plh8os pshfwn oi opoies einai diples
    int vot_count=0;    // plh8os atomwn pou pshfisan parapanw apo mia fora
    if(input==NULL){
        printf("error with output in orcestrator\n");
        return -1;
    }
    fprintf(input,"\nPeople that tried to vote multiple times:\n");
    int counting=numofvoters;
    for(i=0;i<size;i++){
        if(registry[i].voteright>0){ // valid votes
            counting-=registry[i].voteright;
        }
       if(registry[i].voteright>1){
            mult_count+=registry[i].voteright-1;
            vot_count++;
            fprintf(input,"AM: %d   times tried: %d     last time tried: %ld\n",registry[i].AM,registry[i].voteright,registry[i].choice);
       }
    }
    fprintf(input, "\nSum of voters that tried to vote: %d\n",(numofvoters-mult_count));
    fprintf(input,"Valid voters that voted multiple times: %d ",vot_count);
    fprintf(input,"InValid voters: %d ",counting);
    fclose(input);

    // unlink semas..
    ALLsema_unlink();
    // free memory segment
    free(new_args[6]);
    free(new_args[2]);
    free(new_args[4]);
    free(voter_ids);
    //detach memory segment
    shmctl(temp->arrayshmid[0],IPC_RMID,0);
    shmctl(temp->arrayshmid[1],IPC_RMID,0);
    shmctl(temp->arrayshmid[2],IPC_RMID,0);
    shmctl(shmid,IPC_RMID,0);
}


int ALLsema_unlink(){
    sem_unlink(SEM_REG);
    sem_unlink(SEM_TABLE);
    sem_unlink(SEM_QUEUE);
    sem_unlink(SEM_QUEUE2);
    sem_unlink(SEM_WAIT);
    sem_unlink(SEM_COUNTMUT);
    int i=0;
    for(;i<CHECKERS;i++){
        char temp[CHARLIMIT];
        sprintf(temp, "%s%d",SEM_CHECK,i);
        sem_unlink(temp);
        sprintf(temp, "%s%d",SEM_DATA,i);
        sem_unlink(temp);
    }
    return 0;
}

// customized quicksort for the registry
int registry_order(voter* registry,int start,int end){
    int i,j,pivot;
    voter temp;
    if(start<end){
         pivot=start;
         i=start;
         j=end;

         while(i<j){
             while(registry[i].AM<=registry[pivot].AM&&i<end){
                              i++;
            }
             while(registry[j].AM>registry[pivot].AM){
                              j--;
            }
             if(i<j){
                temp.AM=registry[i].AM;
                registry[i].AM=registry[j].AM;
                registry[j].AM=temp.AM;
             }
         }

        temp=registry[pivot];
        registry[pivot]=registry[j];
        registry[j]=temp;
        registry_order(registry,start,j-1);
        registry_order(registry,j+1,end);
        return 0;
    }
}

