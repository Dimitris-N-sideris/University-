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
	
    voter  ticket_tally;
	int voter_choice;		// epilogh 0,1...N
	int shmid;		// shared segment
    int i,size,option;
	if(argc<7){
		printf("not enough Arguments\n");
		return ARG_ERROR;
	}
	while ((option=getopt(argc, argv, "r:s:c:")) != -1){

            switch(option){
            case'r':
                ticket_tally.AM=atoi(optarg);
                break;
			case's':
                shmid=atoi(optarg);
                break;
            case'c':
                voter_choice=atoi(optarg);
                break;			           
            default:
                printf("Arguments not recognizable and it is ignored\n");
                break;
            }
    }
     // Attaching shared memory segment
    segment * temp=shmat(shmid,NULL,0);
    voter * checkertable=  shmat(temp->arrayshmid[2],NULL,0);   
    voter * countertable=  shmat(temp->arrayshmid[1],NULL,0);
    if((int)checkertable==-1||(int)countertable==-1){
        perror("Error with attachment:");
        return -1;
    } 
    size=temp->queuesize;


    // Initializing Semaphores

    sem_t *  checker[3];
    sem_t *  datachecker[3];
    sem_t *  checkerqueue;
    sem_t *  countermutex;
    sem_t *  waiting;
    sem_t *  availableChecker;
    
    for(i=0;i<CHECKERS;i++){
       char named_semaphore[CHARLIMIT];
       sprintf(named_semaphore,"%s%d",SEM_CHECK,i);
       if(sema_init(&checker[i],named_semaphore,0)==0){
            return -1;
        }
        sprintf(named_semaphore,"%s%d",SEM_DATA,i);
       if(sema_init(&datachecker[i],named_semaphore,0)==0){
            return -1;
        }
       
    }
   
    if(sema_init(&waiting,SEM_WAIT,0)==0){
        return -1;
    }
    if(sema_init(&checkerqueue,SEM_QUEUE,3)==0){
        return -1;
    }
    if(sema_init(&countermutex,SEM_COUNTMUT,1)==0){
        return -1;
    }
    if(sema_init(&availableChecker,SEM_OPEN,1)==0){
        return -1;
    }

    // perimene sthn oura mexri na er8ei h seira sou
    sem_wait(checkerqueue); 
    ticket_tally.voteright=0;
    ticket_tally.choice=voter_choice;

    // "vres" to prwto available checker
    sem_wait(availableChecker);
    for(i=0;i<CHECKERS;i++){
       
        if(temp->available_checker[i]==0){
            temp->available_checker[i]=1;
            sem_post(availableChecker);
            break;
        }

    }
 
    checkertable[i].AM=ticket_tally.AM;
    checkertable[i].choice= ticket_tally.choice;

    sem_post(datachecker[i]);
    sem_wait(checker[i]);
    if( checkertable[i].AM!=ticket_tally.AM){
        printf("Problem\n");
    }
    ticket_tally.voteright=checkertable[i].voteright;
    
    
    if(ticket_tally.AM!=-1){
        temp->available_checker[i]=0;           // -1 voter -> dn uparxoun alloi voters -> ase to checker kleisto
    }
   
    sem_post(checkerqueue);


    // putting in the queue of counter
    if(ticket_tally.voteright==0){
        
        sem_wait(countermutex);
        for(i=0;i<size;i++){
            if(countertable[i].AM==0){
                countertable[i].voteright=ticket_tally.voteright;
                countertable[i].choice=ticket_tally.choice;
                countertable[i].AM=ticket_tally.AM;
                break;
            }
        }
        sem_post(countermutex);
       // printf("process %u put data in counter \n",getpid());
        sem_wait(waiting); // wait for counter?
       
    }
    for(i=0;i<CHECKERS;i++){
       sem_close(checker[i]);
       sem_close(datachecker[i]);
    }
    sem_close(waiting);
    sem_close(checkerqueue);
    sem_close(countermutex);
    sem_close(availableChecker);
    if(shmdt(checkertable)==-1){
        perror("Detachment:");
    }
    if(shmdt(countertable)==-1){
        perror("Detachment:");
    }
    if(shmdt(temp)==-1){
        perror("Detachment:");
    }
    //printf("process %u finishing\n",getpid());
    return 0;
}
