#include "JobScheduler.h"
#include <stdarg.h>
#include <pthread.h>

void myprintf(const char* format,...){
	va_list args;
	va_start(args, format);
	printf(format, args);
}

void* queryJob(void* myargs){

	// Prepare for work
	JobScheduler* sch = (JobScheduler*) myargs;
	
	pthread_mutex_lock(&sch->sch_mutex);

	threadSpace* mySpace = get_workspace(sch, sch->threads_waiting);
	sch->threads_waiting++;
	
	if(sch->threads_waiting == sch->execution_threads)
		pthread_cond_signal(&sch->batch_cond_var);
	while(sch->start == 0){

		pthread_cond_wait(&sch->sch_cond_var,&sch->sch_mutex);
	}
	sch->threads_waiting--;

	// Start Working
	while(1){

		sch->threads_waiting++;
		while(emptyJobQueue(sch) || sch->start == 0){
			//printf("hi from thread %d\n",mySpace->threadid);
			
			if(sch->stop == 1){

				sch->exited++;
				if(sch->exited == sch->execution_threads){
				//	printf("I am exiting %d\n",sch->exited);
					pthread_cond_signal(&sch->batch_cond_var);

				}

				pthread_cond_signal(&sch->batch_cond_var);
				pthread_mutex_unlock(&sch->sch_mutex);
				//printf("I am exitin__g %d,__%d \n",sch->exited,sch->execution_threads);
				pthread_exit(NULL);
			}


			if(sch->threads_waiting == sch->execution_threads){
				sch->start = 0;
				pthread_cond_signal(&sch->batch_cond_var);
			}
			
			
			pthread_cond_wait(&sch->sch_cond_var,&sch->sch_mutex);
			
		}
		sch->threads_waiting--;
		// get a job
		mySpace->myjob = popJobQueue(&sch->queue);
		pthread_mutex_unlock(&sch->sch_mutex);

		if(mySpace->myjob == NULL){
			printf("this shouldnt be printed\n");
			pthread_mutex_lock(&sch->sch_mutex);
			//sch->start = 1;
			continue;
		}
		//do job
		mySpace->result = mySpace->myjob->work( sch->ind, mySpace->myjob->input, mySpace->bloomfilter, mySpace->bloomround,mySpace->myjob->version);
		mySpace->bloomround++;
		// store result
		sch->query_result[mySpace->myjob->job_id] = mySpace->result;
		//printf("%s",mySpace->result);
	    deleteNgram(&(mySpace->myjob->input));

	    pthread_mutex_lock(&sch->sch_mutex);
	}	
	
	return NULL;
}

void initJob(Job* myjob, int vers,unsigned int id, Ngram* ngram,char* (*function)(trie* , Ngram* ,int* ,int, int)){

	myjob->version 	= vers;
	myjob->job_id 	= id;
	myjob->input = ngram;
	myjob->work  = function;

}

void copyJob(Job* target, Job* source){

	target->version 		= source->version;
	target->job_id 			= source->job_id;
	target->work 			= source->work;
	target->input 			= source->input;
}

void initJobQueue(JobQueue* myqueue, int initSize){

	myqueue->queries 	= malloc(sizeof(Job)*initSize);
	if(myqueue->queries == NULL){
		printf("not enough memory for job Queue\n");
		exit(MEMERR);
	}
	memset(myqueue->queries, 0, sizeof(Job)*initSize);
	myqueue->queuesize 	= initSize;
	myqueue->first 		= 0;
	myqueue->last 		= 0;
}

int emptyJobQueue(JobScheduler* sch){
	
	if(sch->queue.first == sch->queue.last)
		return 1;

	return 0;
}

void addJobQueue(JobQueue* myqueue, Job* newJob){

	if( myqueue->last >= myqueue->queuesize){
		if(myqueue->first == 0){
			myqueue->queries = realloc(myqueue->queries, myqueue->queuesize*2*sizeof(Job));
			if(myqueue->queries == NULL){

				printf("not enough memory for doubling job Queue\n");
				exit(MEMERR);	
			}
			memset(&myqueue->queries[myqueue->queuesize], 0, sizeof(Job)*myqueue->queuesize);
			myqueue->queuesize 	*= 2;
		}
		else{
			printf("but why?!?!\n");
			//exit(2);
		}
	}
	Job* newlastjob = &myqueue->queries[myqueue->last];
	copyJob(newlastjob, newJob);
	myqueue->last++;

}

Job* popJobQueue(JobQueue* myqueue){
	if(myqueue->first >= myqueue->last)
		return NULL;
	
	Job* popped = &myqueue->queries[myqueue->first];
	myqueue->first++;
	return popped;
}


void destroyJobQueue(JobQueue* myqueue){

	free(myqueue->queries);
}


void 	print_queue(JobScheduler* sch){
	
	JobQueue* que = &(sch->queue);
	int i = que->first;

	for(; i < que->last; i++){
		Job* temp = &(que->queries[i]);
		printf("i: %d  version :%d  jb:%d ngrm:",i, temp->version, temp->job_id);
	//	printNgram(temp->input);
		printf("\n");
	}
}

void init_thread_workspace(JobScheduler* sch, int exec_threads){

	int i;

	sch->thread_workspace = malloc(sizeof(threadSpace)*exec_threads);
	if(sch->thread_workspace == NULL){

		printf("not enough memory for thread workspace creation\n");
		exit(MEMERR);
	}

	for(i=0; i < exec_threads; i++){

		threadSpace* temp = &(sch->thread_workspace[i]);
		temp->bloomfilter = malloc(FILTERSIZE*sizeof(int));	
    	memset(temp->bloomfilter, 0, sizeof(int)*FILTERSIZE);
    	temp->bloomsize  = FILTERSIZE;
    	temp->bloomround = 1;
		temp->myjob 	= NULL;
		temp->result 	= NULL;		
		temp->threadid 	= -1;
	}	
}

threadSpace* get_workspace(JobScheduler* sch, int i){
	threadSpace* temp = &(sch->thread_workspace[i]);
	temp->threadid = i;
	return temp;
}

void destroy_thread_workspace(JobScheduler * sch){
	int i;
	for(i=0; i < sch->execution_threads; i++){

		threadSpace* temp = &(sch->thread_workspace[i]);
    	free(temp->bloomfilter);
	}
	free(sch->thread_workspace);
}



JobScheduler* 	iniialize_scheduler(int exec_threads){

	JobScheduler* sch = malloc(sizeof(JobScheduler));
	if(sch == NULL){
		printf("not enough memory for JobScheduler initialization\n");
		exit(MEMERR);
	}
	sch->tids = malloc(sizeof(pthread_t)*exec_threads);
	if(sch->tids == NULL){
		printf("not enough memory for thread creation\n");
		exit(MEMERR);
	}
	//sch->threadId			= 0;			//helps differciate threads
	sch->execution_threads 	= exec_threads;	//number of threads
	sch->stop 				= 0;			//process must stop
	sch->start 				= 0;
	sch->job_id 			= 0;			//starting jobid -> place of job in table
	sch->threads_waiting 	= 0;
	sch->exited 			= 0;
	sch->query_result 		= malloc(sizeof(char*)*INITSIZE);

	sch->queries_quantity 	= 0;
	sch->queries_maxsize	= INITSIZE;
	init_thread_workspace(sch, exec_threads);
	
	// initialize mutex / condvariables
	pthread_mutex_init(&sch->sch_mutex, NULL);
	//pthread_mutex_init(&sch->batch_mutex, NULL);

	pthread_cond_init(&sch->sch_cond_var, NULL);
	pthread_cond_init(&sch->batch_cond_var, NULL);
	
	initJobQueue( &sch->queue, INITSIZE);

	// create the pool of threads
	int i, error;
	for(i = 0; i < exec_threads ; i++){
		if((error=pthread_create(&sch->tids[i],NULL,queryJob,(void*)sch)) < 0){
			printf("thread creation error\n");
			exit(1);
		}
	}
	// create and initialize thread workspace
	
	return sch;
}



void 	query_table_check(JobScheduler* sch,  int size){

	if(size > sch->queries_maxsize){
		sch->query_result = realloc(sch->query_result,sizeof(char*)*(size*2));
		if(sch->query_result == NULL){
			printf("error with memory\n");
			exit(MEMERR);
		}
		sch->queries_maxsize = size*2;
	}
	//memset(sch->query_result, 0, sizeof(char*)*size + 1);
	sch->queries_quantity = size;
}


void 	submit_job(JobScheduler* sch, Job* myjob){

	myjob->job_id = sch->job_id;
	sch->job_id++;
	addJobQueue(&sch->queue, myjob);
}

void 	execute_all_jobs(JobScheduler* sch){
	//signal all threads to begin
	pthread_mutex_lock(&sch->sch_mutex);

	while(sch->execution_threads > sch->threads_waiting){
		//printf("waiting for threads\n");
		pthread_cond_wait(&(sch->batch_cond_var) ,&sch->sch_mutex);
	}

	// make sure the answer table is ready
	query_table_check(sch,sch->job_id);
	sch->start = 1;

	pthread_cond_broadcast(&(sch->sch_cond_var));
	pthread_mutex_unlock(&sch->sch_mutex);
}

void 			wait_all_tasks_finish(JobScheduler* sch){

	pthread_mutex_lock(&sch->sch_mutex);
	while(sch->start == 1){
		pthread_cond_wait(&(sch->batch_cond_var) ,&sch->sch_mutex);
	}
	
	pthread_mutex_unlock(&sch->sch_mutex);
}

void 			reset_scheduler(JobScheduler* sch){
 // prepares for next batch of commands
	//memset(sch->queue.queries, 0, sizeof(Job)*sch->queue.last);

	sch->queue.last 	= 0;
	sch->queue.first	= 0;
	sch->job_id 		= 0;
}

void 			destroy_scheduler(JobScheduler* sch){
	//printf("destroying\n");

	// wait for threads to exit
	pthread_mutex_lock(&sch->sch_mutex);
	reset_scheduler(sch);
	
	/*
	//pthread_cond_broadcast(&(sch->sch_cond_var));
	//sch->threads_waiting = 0;
	while(sch->exited < sch->execution_threads){
		sch->stop = 1;
		pthread_cond_broadcast(&(sch->sch_cond_var));
		pthread_cond_wait(&(sch->batch_cond_var) ,&sch->sch_mutex);
	}*/
	pthread_mutex_unlock(&sch->sch_mutex);
	destroyJobQueue(&sch->queue);
	destroy_thread_workspace(sch);

	pthread_mutex_destroy(&sch->sch_mutex);
	pthread_cond_destroy(&sch->sch_cond_var);
	pthread_cond_destroy(&sch->batch_cond_var);

	free(sch->query_result);
	free(sch->tids);
	free(sch);
}

