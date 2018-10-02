#ifndef __JobScheduler__
#define	 __JobScheduler__
#include "trie.h"	
#include "Ngram.h"
#include "LinearHash.h"
#include "TopK.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define INITSIZE  1000
#define RESULTS   500
typedef struct trie trie;
typedef struct topk_node topk_node;

typedef struct Job{
	int 			version;
	unsigned int 	job_id;
	Ngram*	 		input;
	char*	(*work)(trie* , Ngram* ,int* ,int,int);

}Job;


typedef struct threadSpace{

	int* 		 bloomfilter;
	unsigned int bloomsize;
	int 		 bloomround;
	char*		 result;
	Job* 		 myjob;
	int 		 threadid;

}threadSpace;

typedef struct JobQueue{
	int 	queuesize;
	int 	first;
	int 	last;
	Job* 	queries;
}JobQueue;

typedef struct 	JobScheduler{
	trie* 		ind;

	int 	 	execution_threads;
	int 		threads_waiting;
	int 		stop;		// Is it over yet? 0 == NO
	int 		start;			//
	int 		exited;

 	pthread_t*  tids;

	threadSpace* 	thread_workspace;	//

	pthread_mutex_t	 	sch_mutex;		// queue mutex

	pthread_cond_t		sch_cond_var;	// threads 	 wait
	pthread_cond_t	 	batch_cond_var;	// scheduler waits

	JobQueue 	queue;

	char** 	query_result;

	int 	queries_quantity;
	int 	queries_maxsize;
	int 	job_id;
	
	//mutex condvar//
    
}JobScheduler;

void* queryJob(void* myargs);

void initJob(Job* myjob, int vers,unsigned int id, Ngram* ngram,char* (*function)(trie* , Ngram* ,int* ,int, int));
void copyJob(Job* job1, Job* job2); 

void 	initJobQueue(JobQueue* myqueue, int initSize);
void 	addJobQueue(JobQueue* myqueue, Job*);
Job* 	popJobQueue(JobQueue* myqueue);
int  	emptyJobQueue(JobScheduler* sch);
void 	print_queue(JobScheduler* sch);


threadSpace* 	get_workspace(JobScheduler* sch, int i);

JobScheduler* 	iniialize_scheduler(int exec_threads);

void 			query_table_check(JobScheduler* sch,  int size);

void 			submit_job(JobScheduler* sch, Job* myjob);
void 			execute_all_jobs(JobScheduler* sch);
void 			wait_all_tasks_finish(JobScheduler* sch);
void 			reset_scheduler(JobScheduler* sch);
void 			destroy_scheduler(JobScheduler* sch);

#endif