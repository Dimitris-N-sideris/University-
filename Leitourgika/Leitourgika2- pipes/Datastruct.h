#ifndef __DATASTRUCT_
#define __DATASTRUCT_
#include <stdio.h>
#include <string.h>
#define SIZEofBUFF 20
#define SSizeofBUFF 6
// orio gia ta char
typedef struct data_struct{
	unsigned long 	custid; 
	char 	FirstName[SIZEofBUFF];
	char 	LastName[SIZEofBUFF];
	char	Street[SIZEofBUFF];
	int 	HouseID;	
	char	City[SIZEofBUFF];
	char	postcode[SSizeofBUFF];
	double  	dapanh;
}typos_data;


void set_data( typos_data* target,const typos_data *source);
int print_instream(FILE * stream,const typos_data* data);
//void print_data( const typos_data* data);
int compare_dapanh( const typos_data*  data1, const typos_data* data2);
unsigned long dataKleidi(const typos_data*  data_);
double  add_data( typos_data*  target,const typos_data*  source);
double  data_dapanh(const typos_data* target);
int 	data_keno(const typos_data* target);


#endif

