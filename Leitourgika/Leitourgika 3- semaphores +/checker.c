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

	int    timeperiod;
    int    shmid;
    int    i;
    int    option;
    int    checkerid;
    if(argc<7){
        printf("not enough Arguments\n");
        return ARG_ERROR;
    }

	while ((option=getopt(argc, argv, "s:d:c:")) != -1){

            switch(option){
            case'd':
                timeperiod=atoi(optarg);
                break;
			case's':
                shmid=atoi(optarg);
                break; 
            case'c':
                checkerid=atoi(optarg);
                break;  		           
            default:
                printf("Arguments not recognizable and it is ignored\n");
                break;
            }
    }
    // attaching memory segment
    segment * temp=shmat(shmid,NULL,0);
    if((int)temp==-1){
        perror("Error with attachment:");
        return -1;
    }
    int size=temp->registrysize;

    voter * checkertable=  shmat(temp->arrayshmid[2],NULL,0);
    voter * registrytable=  shmat(temp->arrayshmid[0],NULL,0);

    if((int)checkertable==-1||(int)registrytable==-1){
        perror("Error with attachment:");
        return -1;
    }
    sem_t* checkerdata,*checkid,*registry,*counter;
    for(i=0;i<CHECKERS;i++){
       if(i==checkerid){
            char named_semaphore[CHARLIMIT];
            sprintf(named_semaphore,"%s%d",SEM_CHECK,i);
            if(sema_init(&checkid,named_semaphore,0)==0){
                return -1;
            }
            //printf("SEMA CHECKERID %d   %s\n",checkerid,named_semaphore);
            sprintf(named_semaphore,"%s%d",SEM_DATA,i);
            if(sema_init(&checkerdata,named_semaphore,0)==0){
                return -1;
            }
            break;
       } 
    }
    // koinoi gia olous tous checkers
    if(sema_init(&registry,SEM_REG,1)==0){
        return -1;
    }
    if(sema_init(&counter,SEM_QUEUE2,temp->queuesize)==0){
        return -1;
    }
    srand(time(NULL));
    voter data;
    do{
    
        sem_wait(checkerdata); // perimene gia data
        data.AM =checkertable[checkerid].AM;
        data.choice=checkertable[checkerid].choice;
       // printf("checke  %u with id %d, processing AM:%d  \n",getpid(),checkerid,data.AM );
        if( data.AM!=-1){


                sem_wait(registry);     // kleise to registry wste na mn to peira3ei allos checker
                // can he vote??
                for(i=0;i<size;i++){
                    if(registrytable[i].AM==data.AM){
                        data.voteright=registrytable[i].voteright;
                        registrytable[i].voteright++;
                        if(registrytable[i].voteright>1){
                            registrytable[i].choice=time(NULL);
                        }
                        break;
                    }
                    else if(registrytable[i].AM>data.AM){   // 
                        data.voteright=-2; // markare oti dn vre8hke
                        break;
                    }
                }
                sem_post(registry);

                if(data.voteright==0){
                    sem_wait(counter);
                }
        }
        else{
           // printf("checker id %d found -1\n",checkerid);
            data.AM=-1;
            data.voteright=0;
           
        }
        checkertable[checkerid].voteright=data.voteright;
        usleep((rand()%timeperiod+1)*TIMECONSTANT);
        sem_post(checkid);
     
    }while(data.AM!=-1);
    

    sem_close(checkid);
    sem_close(counter);
    sem_close(registry);
    sem_close(checkerdata);
    if(shmdt(checkertable)==-1){
        perror("Detachment:");
    }
    if(shmdt(registrytable)==-1){
        perror("Detachment:");
    }
    if(shmdt(temp)==-1){
        perror("Detachment:");
    }
    return 0;
}