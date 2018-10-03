#include	<stdio.h>
#include	<string.h>
#include	<sys/types.h>
#include	<stdlib.h>
#include 	<unistd.h>


int  	main(int argc, char *argv[]){
	int status;
	int pid;
	FILE * pFile, * pFile2;
  		

	if(argc != 2) return -1;
	
	if(atoi(argv[1]) <= 2) { 
	
		pFile = fopen ("myfile1.txt","w");
  		if (pFile!=NULL) {
		printf("\nAnoiksa to 1o arxeio\n\n");
  	  	fputs ("fopen example to file myfile1.txt\n",pFile);
 		}
		printf("1 %d",getpid());
		//sleep(10000);
	}

	

	if(atoi(argv[1]) == 2) { 
	
	pid = fork();
	
	if(pid == 0) {

		pFile2 = fopen ("myfile2.txt","w");
  		if (pFile2!=NULL) {
		printf("Anoiksa to 2o arxeio\n\n");
  	  	fputs ("fopen example to file myfile2.txt\n",pFile2);
 		}
 		printf("2 %d",getpid());
		//sleep(10000);
 		fclose (pFile2);
	}
	else {
		//printf("perimenw to paidi\n");
		wait(&status);
		printf("3 %d",getpid());
		sleep(3990);
 		fclose (pFile);
	}
	}
	
return 0;
}

