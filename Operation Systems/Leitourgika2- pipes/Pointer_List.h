
#ifndef __POINTER__LIST_
#define __POINTER__LIST_
#include <stdio.h>
#include "record.h"

typedef struct typos_komvou
{   tally 		dedomena;
	struct typos_komvou*    epomenos;
	struct typos_komvou*   proigoumenos;
} typos_komvou;	   /*o typos toy kombou ths listas*/



typedef struct info_node
{   int      size;
	int 	 sum;	
	typos_komvou *     arxi; 	/*deikths sto proto komvo tis listas*/
} info_node;     /* o komvos plhroforias ths listas*/

typedef struct info_node  * info_deikti;

typedef struct typos_komvou *typos_deikti;
// Dhmiourgi8ikan / alla3an
void			LIST_move(info_deikti * const linfo,typos_deikti deiktis);
void 			LIST_diadromi(const info_deikti  linfo,int mode,int stream);
void			LIST_diadromif(const info_deikti  linfo,int mode,FILE* stream);
int				LIST_range(info_deikti const linfo,const typos_deikti temp1,const typos_deikti temp2);
typos_deikti   	insertList(tally* record, info_deikti lista);
typos_deikti  	rootInsert(tally* record, info_deikti lista);
typos_deikti 	LIST_eisagogi(info_deikti * const linfo, tally *stoixeio,typos_deikti prodeiktis, int *error);
void 			LIST_percent(const info_deikti linfo,int perc,FILE* stream);
// Xrhsimopoiountai-- dn alla3an
void 			LIST_katastrofi(info_deikti * linfo);
info_deikti 	LIST_dimiourgia();
int				LIST_keni(const info_deikti  linfo);
// Dn xrhsimopoiountai -- dn alla3an
void 			LIST_epomenos(const info_deikti  linfo, typos_deikti * const p, int * const error);
void 			LIST_proigoymenos(const info_deikti linfo, typos_deikti * const p, int * const error);
void 			LIST_first(const info_deikti  linfo, typos_deikti * const first, int * const error);
void 			LIST_last(const info_deikti  linfo, typos_deikti * const last, int * const error);
void 			LIST_anazitisi(const info_deikti  linfo, tally stoixeio,typos_deikti *prodeiktis, int *vrethike);
void 			LIST_periexomeno(const info_deikti  linfo, const typos_deikti p, tally *val,int * error);
void 			LIST_allagi(info_deikti * const linfo, typos_deikti * const p, tally stoixeio,int * error);
void 			LIST_diagrafi(info_deikti * const linfo, typos_deikti *deiktis, int * const error);

#endif /*#ifndef __POINTER__LIST_ */
