#ifndef RECORD_H
#define RECORD_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define     PIPEVOTE    "/home/dimitris/Desktop/DimitrisSideris-Proj2/myfi/myvote"	
#define 	PIPECENT	"/home/dimitris/Desktop/DimitrisSideris-Proj2/myfi/mycenter"
#define     PIPETIME    "/home/dimitris/Desktop/DimitrisSideris-Proj2/myfi/mytime" 
// struct for reading
typedef struct {
   char name[24];
   short electionCenter;
   char valid;
}record;


// struct for holding info
typedef struct{
	char 	name[24];
	int 	quantity;
}tally;

typedef struct{
	char 	name[24];
	double 	realtime;
	double 	cputime;
}temp_time;


void	printTime(FILE * stream,temp_time *temp,int size,int max_depth,int numOfSMs);
void 	setTime(double cput_t,double real_t,int start,int depth,temp_time *temp);
void 	set_record(record*source,tally*target,int mode);
void 	set_data( tally* target,const tally *source);
int 	print_instream(FILE * stream,const tally* data);
int 	print_inwrite(int stream,const tally* data);
int 	compare_name( const tally*  data1, const tally* data2);
int 	comparison( const tally*  data1, const tally* data2);
char* 	dataKleidi(tally*  target);
int  	add_quantity(tally*  target,const tally*  source);
int 	get_quantity(const tally* target);
int 	data_keno(const tally* target);
void 	increase_quantity(tally* target);
int 	compare_quantity(const tally*  data1, const tally* data2);
#endif