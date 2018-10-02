
#include "header.h"

#include "trie.h"
#include "JobScheduler.h"
#include "LinearHash.h"

int wholeLine(FILE* file, char** line, int* size);

int main(int argc, char* argv[]){

	char   inputFile[WORDLIMIT];
    char   queryfile[WORDLIMIT];
    FILE * input;

    clock_t start, end; //Time the programm
    double cpu_time_used;

    start = clock();

    int option;
	 if(argc<5){
        printf("\n\nNot Enough Arguments\n\n");
        return ARG_ERROR;
    }

	while ((option=getopt(argc, argv, "i:q:")) != -1){

            switch(option){
            case'i':
                strcpy(inputFile,optarg);
            case'q':
                strcpy(queryfile,optarg);
                break;                			           
            default:
                printf("Argument %s not recognizable and it is ignored\n", optarg);
                break;
            }
    }

    input = fopen(inputFile,"r");
    if (input==NULL) {
        printf("Cannot open txt file\n");
        return FILEOPENINGERROR;
    }
    
    int size         = 0;
    int type_trie    = 0;
    char * inputline = malloc(sizeof(char)*LINELIMIT);
    int    linesize  = LINELIMIT;

    Ngram *  myNgram;
    createNgram(&myNgram);
  	trie* ind = init_Trie();
   
    JobScheduler*   sch = iniialize_scheduler( THREADS);
    sch->ind = ind;

    while(wholeLine(input, &inputline, &linesize)!=-1){    // reads a line (hopefully)
        size++;
        char *token ;
        char *state;

        for (token = strtok_r(inputline, " \n", &state);
             token != NULL;
             token = strtok_r(NULL, " \n", &state))
        {
          
            addinNgram(myNgram, token);
        }
        if(size == 1){ // first line of input file is either DYNAMIC OR STATIC
        
            if( strcmp(myNgram->wordBuffer[0], STATIC) == 0 ) {
              
                type_trie = TRUE;
            }
            else   type_trie = FALSE;  

        }
        else{
            insert_ngram(ind, myNgram);
        }
		
        resetNgram(myNgram);
    }
    fclose(input);
    
    if(type_trie == TRUE)    start_compress(ind);

    FILE* work   = fopen(queryfile,"r");
    if (work==NULL) {
        printf("Cannot open work file\n");
        return FILEOPENINGERROR;
    }

    int  version = 0, queries     = 0, deletions   = 0, additions   = 0;
    ind->version++;
    int topk_times  = 0;
    topk_node* topk = Topk_init(0);

   // topk_node* topk = Topk_init(type_trie);
    
  
    while(wholeLine(work, &inputline, &linesize) != -1){    // reads a line (hopefully)

        Job  newJob;
        char *token ;
        char *state;
        char *type;
        type = strtok_r(inputline, " \n", &state);
        for (token = strtok_r(NULL, " \n", &state); 
            token != NULL; token = strtok_r(NULL, " \n", &state)){
   
            addinNgram(myNgram,token);
        }
       if(type == NULL){
      //  printf("why?");
        continue;
       }
        if(strcmp(type,QUERY) == 0){
            if(version != 0){
                ind->version++;
                version = 0;
            }
            //printf("found QUERY\n");
            if(myNgram == NULL){
                createNgram(&myNgram);
               // printf("null ngram...\n");
                continue;
            }
            if(type_trie == 1)             // static search
                initJob(&newJob, ind->version, sch->job_id,  myNgram, static_search);
   
            else     
                initJob(&newJob, ind->version, sch->job_id,  myNgram, search);
             
            submit_job(sch, &newJob);
            createNgram(&myNgram);
            queries++;				//also new////////

        }
        else if (strcmp(type,ADD) == 0  ){
            if(type_trie == 1){
                resetNgram(myNgram);
                continue;
            }
            if(version != 1){
                ind->version++;
                version = 1;
            }
		    insert_ngram(ind, myNgram);
            additions++; 
        }
        else if (strcmp(type,DELETE) == 0){
            if(type_trie == 1){
                 resetNgram(myNgram);
                continue;
            }
            if(version != 2){
                ind->version++;
                version = 2;
            }            
            delete_ngram(ind, myNgram);
            deletions++;
        }
        else if (strcmp(type,FINAL) == 0){

           // print_queue(sch);
            execute_all_jobs(sch);
            wait_all_tasks_finish(sch);
            int i;
            for(i=0; i<sch->queries_quantity; i++){
                if(sch->query_result[i] == NULL){
                    printf("NULL\n");
                    continue;
                }

                printf("%s", sch->query_result[i]);

                char *token ;
                char *state = NULL;
                char *topk_ngram;
                topk_ngram = strtok_r(sch->query_result[i], "|", &state);
                topk = TopK_dynamic(topk, topk_ngram);
                for (token = strtok_r(NULL, "|", &state);
                     token != NULL; token = strtok_r(NULL, "|", &state)){

                    topk = TopK_dynamic(topk, token);
                }


                free(sch->query_result[i]);      //!!!! 
                 //!!!! 
                //sch->query_result[i] = NULL;
            }

            topk->topK_num = strtol(myNgram->wordBuffer[0], NULL, 0);


            if(topk->topK_num != 0) {

                if (topk->topK_num > topk->number_of_ngrams)
                    topk->topK_num = topk->number_of_ngrams;

                printf("Top: ");
                if (topk->topK_num > 0) {

                    topk_times = count_time_topk(topk);
                    if(topk_times > topk->number_of_ngrams)
                        topk_times = topk->number_of_ngrams;

                    quicksortGreatestK(topk, 0, topk->number_of_ngrams - 1, topk_times, 0);
                    printArray(topk,topk->topK_num, 0);
                }
                printf("\n");
            }



            TopK_reset(topk,0);


            reset_scheduler(sch);
        }
        else{
            printf("NOT VALID COMMAND\n");
            continue;
        }
        resetNgram(myNgram);
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Programm execution time is %.3lfs (init_file: %s, work_file: %s)\n", cpu_time_used, inputFile, queryfile);
   
    free(inputline);    // not really needed
    
    deleteNgram(&myNgram);
    delete_index(ind);
    fclose(work);
    destroy_scheduler(sch);
    return 0;
}


int wholeLine(FILE* file, char** line, int* size){
    int newsize = *size, read = -1;
    *line[0] = '\0';

   char readBuf[LINELIMIT];

   char* newpointer;
   if(feof(file)){
        return -1;
   }
    memset(readBuf, 0, LINELIMIT);

   while (!feof(file))
   {
    if(fgets(readBuf, LINELIMIT, file) == NULL){
         *size = newsize;
        return read;
    }
    read = 0;
    strcat(*line, readBuf);
   
    int sizeread = strlen(readBuf);

    if (readBuf[sizeread-1] != '\n')  // an o teleutaios xarakthras dn einai allagh grammhs 3anadiavase 
     {
       newsize += LINELIMIT;
       newpointer = realloc(*line, newsize);
       if(newpointer == NULL){
            *size = newsize - LINELIMIT;
            return -1;
       }
       *line = newpointer;
       readBuf[0] = '\0';
     }
     else
       break;
   }
  
   *size = newsize;
   return read;
}
