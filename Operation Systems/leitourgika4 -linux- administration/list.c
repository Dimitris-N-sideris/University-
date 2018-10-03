#include "list.h"


int compare_data(process_info * smaller, process_info * bigger,int mode){

	if(smaller->data[mode]>=bigger->data[mode]){
		return 0;
	}
	else {
		return 1;
	}
}

int copy_data(process_info * target, process_info * source){
	int i;
	for(i=0;i<7;i++){
		target->data[i]=source->data[i];
	}
	return 0;
}

void print_data(process_info * target,FILE * stream){
	int i;
	for(i=0;i<7;i++){
		fprintf(stream, "%d 	",target->data[i] );
	}
	fprintf(stream, "\n");
}


info_deikti LIST_dimiourgia(int mode)
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
	while(todel!=NULL)
	{   
		todel2=todel;
		todel=(todel)->epomenos;
		free(todel2);
	}
    free(*linfo);
    (*linfo)=NULL;
}

int LIST_keni(const info_deikti  linfo){
	return ( linfo->arxi == NULL ); //epistrefei 1 an h lista einai kenh
}


int eisagwgh(info_deikti  linfo,process_info* datas,int mode){
	//print_data(datas,stdout);
	typos_deikti neos_komvos=malloc(sizeof(typos_komvou));	
	if(neos_komvos==NULL){
		
		return -1;
	}
	copy_data(&(neos_komvos->dedomena),datas);

	linfo->size++;
	if(LIST_keni(linfo)){
		linfo->arxi=neos_komvos;
		neos_komvos->epomenos=NULL;
		return 0;
	}
	typos_deikti proigoumenos=linfo->arxi;
	typos_deikti temp=linfo->arxi;
	while(temp!=NULL){
		if(compare_data(&temp->dedomena,datas,mode)){
			break;
		}
		proigoumenos=temp;
		temp=temp->epomenos;
	}
	
	neos_komvos->epomenos=temp;	
	if(temp==linfo->arxi){ 		// put at the start
		linfo->arxi=neos_komvos;	
	}
	else{
		proigoumenos->epomenos=neos_komvos;
		if(temp==NULL){
		}
		else{
			neos_komvos->epomenos=temp;
		}			
		
	}
	
	return 0;
}

void diadromh(const info_deikti  linfo,int processNum){
	typos_deikti temp=linfo->arxi;

	while(temp!=NULL && processNum!=0){
		print_data(&temp->dedomena,stdout);
		
		temp=temp->epomenos;
		processNum--;
	}

}