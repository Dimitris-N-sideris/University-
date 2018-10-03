/*************************************************
Professor           : Delhs Alex
Developer           : Elias Mindrinos
A.M.                : 1115201300102
e-mail              : sdi1300102@di.uoa.gr
**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#define CHAR_LIMIT 1024
//#include "leitourgies.h"

int Find_Arg(int argc, char * argv[], char* parameter, int* pos);
int ArgsM_GET(int argc, char* argv[], int** arguments, int* argNumber);

int main(int argc, char *argv[]) {
    int i, option;
    int argNumber=-1;
    int* args_positions;
   
    i = option = 0;
    argNumber = 0;
    args_positions = NULL;

    option = ArgsM_GET(argc, argv, &args_positions, &argNumber);

    switch(option){
        case 1:     //-cnt
        	if(argc<3){
        		printf("not enough arguments for -cnt \n");
        		return -1;
        	}
        	if(argNumber!=-1){
        		i=args_positions[argNumber-1];
        	}
        	
        	i=startCNT(argv[i]);
       
        	free(args_positions);
            break;
        case 2:     // -lr
        case 3:     // -lw
        	if(argc<4){
        		printf("not enough arguments for -lr||-lw \n");
        		return -1;
        	}
        	/*
				code

			*/
        	free(args_positions);
            break;
        case 4:     // -ps
        	printf("PS\n");
        	if(user_processes()<0){
        		printf("-1 in ps\n");
        		return 0;
        	}
            break;
        case 5:    // -ft 
        	/*
				code
			*/
            break;
        case 6:   // -iostat  
        	if(argc<5){
        		printf("not enough arguments for -iostat\n");
        		return -1;
        	}
        	
        	if(argNumber!=2){
        		printf("error with arguments\n");
        		return -1;
        	}
        	i=args_positions[0];
        	int numberofProcesses= atoi(argv[i]);
        	i=args_positions[1];
        	iostat(argv[i],numberofProcesses);
        	free(args_positions);
            break;
        case 7:     //-netstat
        	if(argc<3){
        		printf("not enough arguments for -netstat\n");
        		return -1;
        	}
        	/*
				code
			*/
        	free(args_positions);
            break;
        default:
        	if(option==0){
        		printf("invalid args\n");
        	}
        	else if(option==-2){
        		printf("Memory error with allocation\n");
        	}
        	else{
        		printf("impossibru\n");
        	}
           return -1;
    }
    /*
    for(i=0;i<argNumber;i++){
        printf("arg position is argv %d   %s\n",args_positions[i],argv[args_positions[i]]);
    }*/

    return 0;
}

//================================================================================================================

int Find_Arg(int argc, char * argv[], char* parameter, int* position) {
    // This function searches the command line if the asked parameter was given.

    int i;

    *position = -1;

    for (i = 1; i < argc; i++) {        // For every argument.

        if (strcmp(argv[i], parameter) == 0) {  //Check if there is the one we want.
            *position =i + 1;
            return i;                   // Return if found.
        }
    }

    return -1;

}

//================================================================================================================

int ArgsM_GET(int argc, char* argv[], int** arguments, int* argNumber) {
    // This function determines if the arguments were as expected and returns the result. 

    int pos;

    //==========================================================================================================

    if (Find_Arg(argc, argv, "-cnt", &pos) >= 0) {
        printf("!!!\n\n");
        *argNumber=1;
        *arguments=malloc(sizeof(int));
        if(*arguments==NULL){
            printf("Error with memory allocation\n");
            return -2;
        }
        (*arguments)[0]=pos;
        return 1;
    }
    else if (Find_Arg(argc, argv, "-lr", &pos) > 0){
        int temp = pos;
        if(Find_Arg(argc, argv, "-type", &pos)  > 0){

            *argNumber=3;
            *arguments=malloc(sizeof(int)*3);
            if(*arguments==NULL){
                printf("Error with memory allocation\n");
                return -2;
            }
            (*arguments)[0]=temp;
            (*arguments)[1]=pos;
            (*arguments)[2]=argc-1;
            return 2;
        }
        *argNumber=2;
        *arguments=malloc(sizeof(int)*2);
        if(*arguments==NULL){
            printf("Error with memory allocation\n");
            return -2;
        }
        (*arguments)[0]=temp;
        (*arguments)[1]=argc-1;
        return 2;
    }
    else if(Find_Arg(argc, argv, "-lw", &pos)  >0){
        int temp = pos;
        if(Find_Arg(argc, argv, "-type", &pos)  > 0){
           
            *argNumber=3;
            *arguments=malloc(sizeof(int)*3);
             if(*arguments==NULL){
            printf("Error with memory allocation\n");
            return -2;
        }
            (*arguments)[0]=temp;   
            (*arguments)[1]=pos;
            (*arguments)[2]=argc-1;
            return 3;
        }
       
        *argNumber=2;
        *arguments=malloc(sizeof(int)*2);
        if(*arguments==NULL){
            printf("Error with memory allocation\n");
            return -2;
        }
        (*arguments)[0]=temp;  
        (*arguments)[1]=argc-1;


        return 3;
    }   
    else if (Find_Arg(argc, argv, "-ps", &pos)  > 0){    // dn xrhsimopoiei arguments
        *argNumber=0;
        *arguments=NULL;
        return 4;
    } 
    else if (Find_Arg(argc, argv, "-ft", &pos)  > 0){// dn xrhsimopoiei arguments
        *argNumber=0;
        *arguments=NULL;
        return 5;
    }
    else if (Find_Arg(argc, argv, "-iostat", &pos) > 0){
        *arguments=malloc(sizeof(int)*2);
        if(*arguments==NULL){
            printf("Error with memory allocation\n");
            return -2;
        }
        *argNumber=2;
       
        
        if(Find_Arg(argc, argv, "-k", &pos)  > 0){
            (*arguments)[0]=pos;
        }
        else{
            (*arguments)[0]=-1;
        }
        if(Find_Arg(argc, argv, "-f", &pos) > 0){
            (*arguments)[1]=pos;
        }
        else{
           (*arguments)[1]=-1;     
        }
        return 6;
    }
    else if (Find_Arg(argc, argv, "-netstat", &pos) > 0){   // dn xrhsimopoiei arguments
        *argNumber=1;
        *arguments=malloc(sizeof(int));
        if(*arguments==NULL){
            printf("Error with memory allocation\n");
            return -2;
        }
         (*arguments)[0]=pos;
        return 7;
    }
    else {
        printf("Wrong argument choices: -cnt -type -ps -ft -iostat -netstat\n");
        return 0;
    }

    return -1;

}