

#ifndef __ProjectHeader__
#define	 __ProjectHeader__
#include "JobScheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define THREADS 	4

#define DELETE 				"D"
#define ADD					"A"
#define QUERY				"Q"
#define FINAL				"F"
#define STATIC				"STATIC"
#define DYNAMIC 			"DYNAMIC"

#define WORDLIMIT 			50		// Start point of dynamic node strings
#define LINELIMIT 			2000	// Start point of dynamic general strings

#define numofhash		    4
#define FILTERSIZE		    300000


#define STARTNODESIZE  		3		// Start of each sub node table
#define TABLESIZE			10		// Start of dynamic table (panta megalutero apo 2)

#define OK_SUCCESS  		0
#define	ARG_ERROR 			1
#define MEMERR  			2
#define BADUSE				3
#define NOTFOUND			4 
#define FILEOPENINGERROR	-1

#define TRUE 				1
#define FALSE 				0

#endif
