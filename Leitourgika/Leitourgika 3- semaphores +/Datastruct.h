#ifndef __DATASTRUCT_
#define __DATASTRUCT_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#define SEM_REG			"/semaregisty"
#define	SEM_TABLE		"/sematable"
#define SEM_QUEUE 		"/semaqueue"
#define SEM_QUEUE2		"/semacounter"
#define	SEM_CHECK		"/semacheck"
#define	SEM_DATA		"/semadata"
#define SEM_COUNTMUT   	"/semacountermutex"
#define SEM_WAIT	    "/semacountwait"
#define SEM_OPEN 		"/semavailable"
#define CHARLIMIT 		256
#define CHECKERS 		3
#define SIZEofBUFF 		20
#define SSizeofBUFF 	6
#define TIMECONSTANT 	1000
#define ARG_ERROR		-2
// orio gia ta char
#define	SEM_DATA1	"/semadata1"
#define	SEM_DATA2	"/semadata2"
#define	SEM_CHECK1	"/semacheck1"
#define	SEM_CHECK2	"/semacheck2"

typedef struct data_struct{
	int registrysize;
	int queuesize;
	int choices;
	int arrayshmid[3];
	int available_checker[CHECKERS];
}segment;

union semun {

   int val;                  /* value for SETVAL */
   struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
   unsigned short *array;    /* array for GETALL, SETALL */

};

typedef struct voter{
    int     AM;
    int     voteright;
    long    choice;
}voter;

int sema_init(sem_t ** sp,char * semname,int init_value){
    *sp = sem_open(semname,O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,init_value);
    if(*sp!=SEM_FAILED){
      // printf("sema %s created\n",semname);
       
        return 1;
    }
    else if(errno==EEXIST){
        *sp=sem_open(semname,0);
        return 1;
    }
    else{
    	printf("Error sema_init %s\n",semname);
        return 0;
    }
}
#endif

