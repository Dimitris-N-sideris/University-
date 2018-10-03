#ifndef __LEITOURGIES_
#define __LEITOURGIES_

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<time.h>
#include 	<sys/stat.h>
#include	<sys/types.h>
#include 	<dirent.h>
#include 	<errno.h>
#include 	<unistd.h>
#include 	<fcntl.h>
#include	"list.h"

#define 	CHAR_LIMIT 1024
#define 	CLOCKS_PER 100
#define 	CMDLINE		"cmdline"	
#define 	FILE5		"io"
#define 	MEMORY		"mem"
#define 	STATUS		"stat" //"/status"

#define 	PROC		"/proc" 
#define 	PID			"PID"
#define 	RCHAR		"RCHAR"
#define 	WCHAR		"WCHAR"
#define 	SYSR		"SYSR"
#define 	SYSW		"SYSW"
#define 	RFS			"RFS"
#define 	WFS			"WFS"

int startCNT(char * fileDirectory);
int cntFunction(char * directory,int * results);
int crtPath(char** target,char * path, char * next);
int process_data(char * directory,unsigned long* kb_used,double* cpu_time);
int user_processes();
int iostat(char * mode, int numProcesses);
#endif