#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "Datastruct.h"


int main(int argc, char* argv[]){

	char   inputFile[CHARLIMIT];
    char   outputFile[CHARLIMIT];
    int    timeperiod,shmid,i,option;

    if(argc<9){
        printf("not enough Arguments\n");
        return ARG_ERROR;
    }
	while ((option=getopt(argc, argv, "i:s:d:o:")) != -1){

            switch(option){
            case'd':
                timeperiod=atoi(optarg);
                break;
            case'i':
                strcpy(inputFile,optarg);
                break;
			case'o':
                strcpy(outputFile,optarg);
                break;
            case's':
                shmid=atoi(optarg);
                break;			           
            default:
                printf("Arguments not recognizable and it is ignored\n");
                break;
            }
    }
     // Attaching shared memory segment
    segment * temp=shmat(shmid,NULL,0);
    if((int)temp==-1){
        perror("Error with attachment:");
        return -1;
    }
    int size=temp->registrysize;
    int queuesize=temp->queuesize;
    int num_choices=temp->choices;
    int* tally_holder;

    voter * countertable=  shmat(temp->arrayshmid[1],NULL,0);
    voter * registrytable=  shmat(temp->arrayshmid[0],NULL,0);

    if((int)registrytable==-1||(int)countertable==-1){
        perror("Error with attachment:");
        return -1;
    }
    tally_holder=malloc(sizeof(int)*num_choices);
    if(tally_holder==NULL){
        printf("Error with memory allocation\n");
        return -1;
    }
    for(i=0;i<num_choices;i++){
        tally_holder[i]=0;
    }
    sem_t* registry,*counter,*waiting;
    if(sema_init(&registry,SEM_REG,1)==0){
        return -1;
    }
      if(sema_init(&counter,SEM_QUEUE2,queuesize)==0){
        return -1;
    }
  
    if(sema_init(&waiting,SEM_WAIT,1)==0){
        return -1;
    }
    srand(time(NULL));
    int count =CHECKERS;
    int sumofvalid=0;

    do{
      
        for(i=0;i<queuesize;i++){
      
            if(countertable[i].AM==-1){
               

                count--;
                countertable[i].choice=0;
                countertable[i].voteright=0;
                countertable[i].AM=0;
                sem_post(counter);
                sem_post(waiting);
                continue;
            }
            else if(countertable[i].AM>0){
                   // printf("%d voted %ld\n",countertable[i].AM,countertable[i].choice);
                    sumofvalid++;
                    tally_holder[countertable[i].choice]++;
                    countertable[i].choice=0;
                    countertable[i].voteright=0;
                    countertable[i].AM=0;
                    sem_post(counter);
                    sem_post(waiting);
                }
            else{ 
                //countertable[i].choice=0;
                //countertable[i].voteright=0;
                countertable[i].AM=0;
               continue;
            }
            
            usleep(TIMECONSTANT);
        }

        usleep((rand()%timeperiod+1)*TIMECONSTANT);
    }while(count); // efoson dn exoun perasei oloi oi pshfoforoi pou htan sto input
    FILE * output;
    output=fopen(outputFile,"w");
    if(output==NULL){
        printf("Error creating %s file",outputFile);
        return -1;
    }
    fprintf(output,"Number of valid votes %d\n\n",sumofvalid);
    for(i=0;i<num_choices;i++){ 
        double percent = (double)tally_holder[i]/(double)sumofvalid;
        fprintf(output,"Choice %d got %d votes  and with percentage %d/%d = %f \n",i,tally_holder[i],tally_holder[i],sumofvalid,percent);
    }
    fclose(output);
    sem_close(counter);
    sem_close(waiting);
    sem_close(registry);
    //sem_close(checkerdata);
    if(shmdt(registrytable)==-1){
        perror("Detachment:");
    }
    if(shmdt(countertable)==-1){
        perror("Detachment:");
    }
    if(shmdt(temp)==-1){
        perror("Detachment:");
    }

    return 0;
}