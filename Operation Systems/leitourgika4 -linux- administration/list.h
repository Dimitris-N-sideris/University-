#include <stdio.h>
#include <stdlib.h>

typedef struct process_info
{   
	int data[7];

} process_info;	



typedef struct typos_komvou
{  
	process_info	 		dedomena;
	struct typos_komvou*    epomenos;
} typos_komvou;	   /*o typos toy kombou ths listas*/



typedef struct info_node
{  
	int      size;	
	typos_komvou *     arxi; 	/*deikths sto proto komvo tis listas*/
} info_node;     /* o komvos plhroforias ths listas*/

typedef struct info_node  * info_deikti;

typedef struct typos_komvou *typos_deikti;



int compare_data(process_info * smaller, process_info * bigger,int mode);
int copy_data(process_info * target, process_info * source);
void print_data(process_info * target,FILE * stream);

info_deikti LIST_dimiourgia();
void LIST_katastrofi(info_deikti * linfo);
int LIST_keni(const info_deikti  linfo);
int eisagwgh(info_deikti  linfo,process_info* datas,int mode);
void diadromh(const info_deikti  linfo,int processNum);