#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "Pointer_List.h"
#include "record.h"



typos_deikti eisagogi_arxi(info_deikti * const,tally*, int *);
typos_deikti eisagogi_meta(info_deikti *  const,tally*, typos_deikti, int *);


info_deikti LIST_dimiourgia( )
{
    info_deikti linfo;
    linfo = malloc(sizeof(info_node));
    linfo->size = 0;
	linfo->arxi = NULL;
	return linfo;
}


void LIST_katastrofi(info_deikti * linfo)
{/*	Pro: 	  Dhmioyrgia listas
  *	Meta: 	  Katastrofi ths listas kai apodesmeusi sto telos kai tou komvou linfo */
	int list_size=(*linfo)->size;
	typos_deikti todel,todel2;
	todel= (*linfo)->arxi;
	while(list_size)
	{   todel2=todel;
		todel=(todel)->epomenos;
		free(todel2);
		list_size--;
	}
	(*linfo)->arxi = NULL;
    free(*linfo);
    (*linfo)=NULL;
}

int LIST_keni(const info_deikti  linfo){
	return ( linfo->arxi == NULL ); //epistrefei 1 an h lista einai kenh
}



void LIST_diadromi(const info_deikti  linfo,int mode,int stream){
	
	typos_deikti temp;
    int list_size=linfo->size;
	
	
	if(LIST_keni(linfo))
	{   
		return;
	}
	if(mode){
		temp=(linfo)->arxi;
		while(list_size)
		{   
			
			print_inwrite(stream,&(temp->dedomena));
			
			temp=temp->epomenos;
			list_size--;
			
		}
	}
	else{
		temp=(linfo)->arxi->proigoumenos;
		while(list_size)
		{   
			print_inwrite(stream,&(temp->dedomena));
			temp=temp->proigoumenos;
			list_size--;
	
		}
	}
	//fprintf(stdout,"Finished putting in pipe\n");
}


void LIST_diadromif(const info_deikti  linfo,int mode,FILE* stream){
	
	typos_deikti temp;
    int list_size=linfo->size;
	
	
	if(LIST_keni(linfo))
	{   
		return;
	}
	if(mode){
		temp=(linfo)->arxi;
		while(list_size)
		{   
			print_instream(stream,&(temp->dedomena));
			temp=temp->epomenos;
			list_size--;
			
		}
	}
	else{
		temp=(linfo)->arxi->proigoumenos;
		while(list_size)
		{   
			print_instream(stream,&(temp->dedomena));
			temp=temp->proigoumenos;
			list_size--;
	
		}
	}
	//fprintf(stdout,"Finished putting in pipe\n");
}


int		LIST_range(info_deikti const linfo,const typos_deikti temp1,const typos_deikti temp2){
		
		typos_deikti start=NULL;
		typos_deikti end=NULL;
		int count=0;
		if(compare_name(&temp2->dedomena,&temp1->dedomena)){
			start=temp2->proigoumenos;
			end=temp1;
		}
		else {
			start=temp1->proigoumenos;
			end=temp2;
		}
		while(start!=end && start!=NULL){
			count++;
			print_instream(stdout,&start->dedomena);
			start=start->proigoumenos;
		}
		fprintf(stdout,"\n");
		return count;
}

void 	LIST_percent(const info_deikti linfo,int perc,FILE* stream){
		fprintf(stream,"These centers have around %d percent of the votes\n\n",perc);
		typos_deikti start=linfo->arxi;
		int size=linfo->size;
		int count=0;
		int percent= (linfo->sum*perc)/100;
		if(start==NULL)  return ;
		
		while(size && count<=percent ){
			
			count+=get_quantity(&(start->dedomena));
			print_instream(stream,&(start->dedomena));
			start=start->epomenos;
			size--;	
		}	
		
}


typos_deikti   insertList(tally* record, info_deikti lista){
	
	int list_size=lista->size;   
	int error=0;
	typos_deikti temp=lista->arxi;
	
	if(LIST_keni(lista)){
		temp=LIST_eisagogi(&lista,record,NULL, &error);
	}
	else{	
			// psa3e sth list gia kapoio paromoio tally
		while(list_size){
			int compared=comparison(&temp->dedomena,record);	
			
			if(compared==0){						// an uparxei
				add_quantity(&temp->dedomena,record);	//  ananewse
				break;
			}
			else if(compared>0){ 					// an einai idia megalutero
		
				if(list_size!=lista->size) {
					temp=eisagogi_meta(&lista,record,temp->proigoumenos,&error);
				}	
				else{
					temp=LIST_eisagogi(&lista,record,NULL, &error);
				}
				break;
			}
			
			temp=temp->epomenos;
			list_size--;
		}
		if(list_size==0){
			temp=eisagogi_meta(&lista,record,lista->arxi->proigoumenos,&error);
		}		
	}
	
	//LIST_diadromi(lista,&error);
	if(error){
		return NULL;
	}
	
	return temp;
}




typos_deikti   rootInsert(tally* record, info_deikti lista){
	
	int list_size=lista->size;   
	int error=0;
	typos_deikti temp=lista->arxi;
	lista->sum+=get_quantity(record);

	if(LIST_keni(lista)){
		temp=LIST_eisagogi(&lista,record,NULL, &error);
	}
	else{	
			// psa3e sth list gia kapoio paromoio tally
		while(list_size){
			
			
			if(compare_name(&temp->dedomena,record)){						// an uparxei
				add_quantity(&temp->dedomena,record);	//  ananewse
				LIST_move(&lista,temp);
				break;
			}
			temp=temp->epomenos;
			list_size--;
		}
		if(list_size==0){
			temp=eisagogi_meta(&lista,record,lista->arxi->proigoumenos,&error); // ftia3to kai kanto move
			LIST_move(&lista,temp);
		}		
	}
	
	//LIST_diadromi(lista,&error);
	if(error){
		return NULL;
	}
	
	return temp;
}




typos_deikti LIST_eisagogi(info_deikti * const linfo, tally *stoixeio,
					typos_deikti prodeiktis, int *error)
{/*	Pro: 		Dhmiourgia listas
  * Meta: 		Eisagetai to "stoixeio" meta ton "prodeikti",an einai null autos to stoixeio mpainei
                stin arxi tis listas allios mpainei meta apo ton komvo pou deixnei autos */
	*error=0;
	if(prodeiktis==NULL)
		return eisagogi_arxi(linfo, stoixeio, error);
	else
		return eisagogi_meta(linfo,stoixeio,prodeiktis, error);
}

typos_deikti eisagogi_arxi(info_deikti *  const linfo,tally *stoixeio, int *error)
{/*	Pro: 		Dhmiourgia listas
  *	Meta: 		O kombos me ta dedomena stoixeio exei eisax8ei sthn arxh ths listas */

	typos_deikti prosorinos; /*Deixnei ton neo kombo pou prokeitai na eisax8ei*/

	prosorinos = malloc(sizeof(typos_komvou));
	if ( prosorinos == NULL )
	{   *error=1;
		return prosorinos;
	}
	set_data(&prosorinos->dedomena, stoixeio);
	
    prosorinos->epomenos = (*linfo)->arxi;
    if ((*linfo)->arxi != NULL){
		prosorinos->proigoumenos=(*linfo)->arxi->proigoumenos;
		
		if((*linfo)->size==1){		// tetrimmenh katastash otan exoume ena komvo kai vazoume sthn arxh
			(*linfo)->arxi->epomenos=prosorinos;
		}
		else{    /*allazei ton deixth pou deixth ston epomeno tou prohgoumenou 
			apo ton arxiko komvo gia na diathrh8ei o kuklos*/
			(*linfo)->arxi->proigoumenos->epomenos=prosorinos;
		}
		(*linfo)->arxi->proigoumenos = prosorinos;
    }
    else{
			prosorinos->epomenos =prosorinos;
			prosorinos->proigoumenos=prosorinos;
	}
	
    (*linfo)->arxi = prosorinos;
	(*linfo)->size ++;
	//(*linfo)->sum+=stoixeio->dapanh;	
    return prosorinos;
}

typos_deikti eisagogi_meta(info_deikti *  const linfo,tally *stoixeio,
					typos_deikti prodeiktis, int *error)
{ /* Pro: 		Dhmioyrgia listas
   * Meta: 		O kombos me ta dedomena stoixeio eisagetai
   *			meta ton kombo pou deixnei o prodeikths*/
	//int list_size=(*linfo)->size;
	typos_deikti prosorinos; /*Deixnei ton neo kombo pou prokeitai na eisax8ei*/

	prosorinos = malloc(sizeof(typos_komvou));
	if ( prosorinos == NULL )
	{   *error = 1;
		return prosorinos;
	}
	else
	{   
		//typos_deikti temp=(*linfo)->arxi;	



		set_data(&(prosorinos->dedomena), stoixeio);
		prosorinos->epomenos = prodeiktis->epomenos;
		prosorinos->proigoumenos = prodeiktis;
		if(prosorinos->epomenos!=NULL)
			prosorinos->epomenos->proigoumenos = prosorinos;
		prodeiktis->epomenos = prosorinos;
		(*linfo)->size ++;
		//(*linfo)->sum+=stoixeio->dapanh;	
	}

	return prosorinos;
}

void LIST_move(info_deikti * const linfo,typos_deikti deiktis){
	if((*linfo)->size==1) return;
	 // afairesh apo thn lista
	 
	
	typos_deikti temp=(*linfo)->arxi;	
				// epanatopo8ethsh
	while(temp!=deiktis){
			
			if(compare_quantity(&(temp->dedomena),&(deiktis->dedomena))){
				break;
			}
			temp=temp->epomenos;
	}

	if(temp==deiktis)	return;	 		// mn to metakinhseis


	// afairese to prin to metakinhseis
	typos_deikti temp1	=	deiktis->epomenos;
	temp1->proigoumenos	=	deiktis->proigoumenos;
	temp1->proigoumenos->epomenos = temp1;
	// valto sth 8esh prin apo ekei pou deixnei to temp
	
	if(temp==(*linfo)->arxi){		// valto sthn arxh 
			
			(*linfo)->arxi=deiktis;
			deiktis->epomenos=temp;
			deiktis->proigoumenos=temp->proigoumenos;
			temp->proigoumenos->epomenos=deiktis;
			temp->proigoumenos=deiktis;	
	}
	
	else{ 						// se opoiadhpote allh periptwsh
			deiktis->epomenos=temp;
			deiktis->proigoumenos=temp->proigoumenos;
			temp->proigoumenos->epomenos=deiktis;
			temp->proigoumenos=deiktis;					
		}
		
}
		
	


 // DN XRHSIMOPOIOUNTAI
void LIST_diagrafi(info_deikti * const linfo, typos_deikti *deiktis, int * const error)
{/*	Pro: 		Dhmiourgia Listas
  *	Meta: 		Diagrafetai to stoixeio ths listas pou deixnei o "deiktis" */

    typos_deikti prosorinos,previous;
	*error=0;

	if (LIST_keni(*linfo)|| (*deiktis==NULL))
	  {	*error=1;
	    return;
     }
	if((*linfo)->arxi==*deiktis)  //an einai o protos kombos tis listas
	{   prosorinos=*deiktis;
	    *deiktis=prosorinos->epomenos;
		(*linfo)->arxi=*deiktis;
        if((*deiktis)!=NULL)
          (*deiktis)->proigoumenos=NULL;
	}
	else        //se kathe alli periptosi
	{  prosorinos=*deiktis;
       previous=prosorinos->proigoumenos;
       *deiktis=prosorinos->epomenos;
       previous->epomenos=prosorinos->epomenos;
	   if((*deiktis)!=NULL)
	      (*deiktis)->proigoumenos = previous;
	}
	free(prosorinos);
    prosorinos = NULL;
	(*linfo)->size--;
}

void LIST_epomenos( info_deikti  const linfo, typos_deikti * const p, int * const error)
{/*	Pro: 		Dhmiourgia listas
  *	Meta:		Epistrefei ton epomeno komvo tou "p" kai sto error 0,an o p einai null tote epistrefei
                sto error 2 allios an den iparxei epomenos epistrefei sto error 1 */
	*error = 0;
	if((*p)!=NULL)
	 { if((*p)->epomenos!=NULL)
		 *p=(*p)->epomenos;
       else
         *error=1;
      }
	else
	   *error=2;
}

void LIST_proigoymenos(const info_deikti linfo, typos_deikti * const p, int * const error)
{/*	Pro: 		Dhmiourgia listas
  *	Meta:		Epistrefei ton prohgoumeno komvo tou "p" kai sto error 0,an o p einai null tote epistrefei
                sto error 2 allios an o p einai o protos komvos tis listas epistrefei sto error 1 */
	*error = 0;
	if  ((*p)!=NULL)             /* lista oxi adeia */
	{   if ((*p)==(linfo->arxi)->epomenos)			/* p deixnei ston deutero kombo */
   	  		*p=linfo->arxi;
		else if(*p==linfo->arxi)					/* p deixnei ston proto kombo */
			*error=1;
		else                       /* se kathe alli periptosi */
			*p=(*p)->proigoumenos;
	 }
	else
        *error=2;
}

void LIST_first(info_deikti  const linfo, typos_deikti * const first, int * const error)
{ /*Pro: 		Dhmiourgia Listas
   *Meta:		Epistrefei ton  proto komvo ths listas diaforetika epistrefei 1 sto error */
    *error=0;
	*first=linfo->arxi;
	if(LIST_keni(linfo))
		*error=1;
}

void LIST_last(info_deikti  const linfo, typos_deikti * const last, int * const error)
{/*	Pro: 		Dhmiourgia listas
  *	Meta:		Epistrefei ton  teleutaio komvo ths listas diaforetika 1 sto error an einai kenh auti */

	typos_deikti prosorinos;
	*error = 0;
	*last=NULL;
	if(LIST_keni(linfo))
		*error=1;
	else
	{   prosorinos=linfo->arxi;
	    while (prosorinos->epomenos != NULL)
			prosorinos=prosorinos->epomenos;
		*last=prosorinos;
	}
}
/*
void LIST_anazitisi(info_deikti  const linfo, tally stoixeio,
					typos_deikti *prodeiktis, int *vrethike)
{*	Pro: 		Dhmiourgia listas
  *	Meta:		Anazhta an yparxei sthn lista to stoixeio,an yparxei to vrethike ginetai 1
	tally temp;
	typos_deikti trexon;
	int error;

	error=0;
	trexon = linfo->arxi;
	*prodeiktis = NULL;
	*vrethike = 0;
	while ( (!(*vrethike)) && (trexon!=NULL))
	{   LIST_periexomeno(linfo,trexon,&temp,&error);
		if (TSlist_iso(temp, stoixeio)) //an vrethei to stoixeio
			*vrethike =1;
		else
		{   *prodeiktis = trexon;
			LIST_epomenos(linfo, &trexon, &error);
			if(error)  //an den iparxei epomenos
               trexon=NULL;
		}
	}
}
*/
