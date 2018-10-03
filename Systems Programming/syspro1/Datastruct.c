
#include "Datastruct.h"


int 		empty_list(graphNode* node){
	if(node->payments->arxi==NULL){
		return 0;
	}
	return 1;
}


int 	add_akmh(graphNode * source, double posothta,graphNode* data){
	
	
	lista_akmwn* target=source->payments;
	akmh* temp=target->arxi;
	akmh* proigoumenh_akmh=target->arxi;
	
	while(temp!=NULL){
		if(temp->nodeptr==data){	// uparxei hdh
			printf("Updating transaction \n");
			temp->poso+=posothta;
			data->income+=posothta;
			source->outcome+=posothta;
			return 0;
		}
		proigoumenh_akmh=temp;
		temp=temp->next;
			
	}

	akmh* nea_akmh=malloc(sizeof(akmh));	// desmeuse xwro gia th nea akmh
	if(nea_akmh==NULL){
		fprintf(stderr, "error with malloc\n");
		return -1;
	}
	nea_akmh->nodeptr=data;		// arxikopoihse thn
	nea_akmh->poso=posothta;
	nea_akmh->next=NULL;

	source->outcome+=posothta;
	data->income+=posothta;

	if(target->arxi==NULL){
		target->arxi=nea_akmh;
		return 0;
	}
	proigoumenh_akmh->next=nea_akmh;
	return 0;
}

int 	remove_akmh(graphNode * source,graphNode* target){
	
	akmh* temp=source->payments->arxi;

	if(temp==NULL){
		printf("No transactions in %7.7d\n",source->keyName);
		return 1;
	}
	if(temp->nodeptr==target){
		source->payments->arxi=temp->next;
		source->outcome-=temp->poso;
		target->income-=temp->poso;
		free(temp);
		return 0;
	}
	while(temp->next->nodeptr!=target){
		temp=temp->next;
		if(temp==NULL){
			break;
		}
	}
	if(temp==NULL){
		printf("Couldn't find a transaction with %7.7d in node %7.7d\n",target->keyName,source->keyName);
		return 1;
	}
	source->outcome-=temp->next->poso;
	target->income-=temp->next->poso;
	akmh* to_erase=temp->next;
	temp->next=temp->next->next;
	free(to_erase);
	
	return 0;
}


lista_akmwn*create_list(){
	lista_akmwn* temp=malloc(sizeof(lista_akmwn));
	if(temp==NULL){
		fprintf(stderr, "error with malloc\n");
		return temp;
	}
	temp->size=0;
	temp->arxi=NULL;
	return temp;
}

graphNode*	create_graphNode(int id){
	graphNode* temp=malloc(sizeof(graphNode));
	if(temp==NULL){
		return NULL;
	}
	temp->keyName=id;
	temp->check=0;
	temp->income=0;
	temp->outcome=0;
	temp->payments=create_list();
	if(temp->payments==NULL){
		free(temp);
		return NULL;
	}
	return temp;
}

double	lookup(graphNode* node,int mode){
	
	switch(mode){
		case 0:	//in
			return node->income;
			break;
		case 1:	//out
			return node->outcome;
			break;
		default:	// sum
			return( node->income)-(node->outcome);	
	}
}

void delete_all(akmh* target){
	if(target==NULL){
		return;
	}
	delete_all(target->next);
	free(target);
}

int 		delete_Node(graphNode* target){
	
	delete_all(target->payments->arxi);
	free(target->payments);
	free(target);
	return 0;
}

void 		print_data(FILE* stream,graphNode* node){
	akmh* temp=node->payments->arxi;
	fprintf(stream, " vertex(%7.7d) = ",node->keyName);
	while(temp!=NULL){
		fprintf(stream, "(%7.7d,%.3f)",temp->nodeptr->keyName,temp->poso);
		temp=temp->next;
		if(temp!=NULL){
			fprintf(stream, ",");	
		}
	}
	fprintf(stream, "\n");
}


int pathfind(graphNode* current,graphNode* target,lista_akmwn* list){
	
	if(current==NULL){
		return 1;
	}
	akmh* akmes=current->payments->arxi;
	
	while(akmes!=NULL){

		if(akmes->nodeptr==NULL){
			continue;
		}
		if(akmes->nodeptr==target){
			addakmh(list,akmes->nodeptr);
			printList(list);
			printf("\n");
			removeakmh(list,akmes->nodeptr);
			return 0;
		}
		if(akmes->nodeptr->check==1){
			akmes=akmes->next;
			continue;
		}
		akmes->nodeptr->check=1;
		addakmh(list,akmes->nodeptr);
		int result=pathfind(akmes->nodeptr,target,list);
		removeakmh(list,akmes->nodeptr);
		if(result==0){
			return 0;
		}
		akmes=akmes->next;
	}	
	return 1;
}


int addakmh(lista_akmwn* list,graphNode* data){
	akmh *nea_akmh=malloc(sizeof(akmh));
	if(nea_akmh==NULL){
		printf("malloc error\n");
		return -1;
	}
	nea_akmh->nodeptr=data;
	nea_akmh->next=NULL;
	akmh* temp=list->arxi;
	list->size++;
	if(temp==NULL){
		list->arxi=nea_akmh;
		return 0;
	}
	while(temp->next!=NULL){
		temp=temp->next;
	}
	temp->next=nea_akmh;
	return 0;
}

int  removeakmh(lista_akmwn* list,graphNode* data){

	akmh* temp=list->arxi;
	if(temp==NULL){
		return 1;
	}
	if(temp->nodeptr==data){
		list->size--;
		list->arxi=temp->next;
		free(temp);
		return 0;
	}
	while(temp->next->nodeptr!=data){
		temp=temp->next;
		if(temp==NULL){
			break;
		}
	}
	if(temp!=NULL){
		list->size--;
		akmh* target=temp->next;
		temp->next=temp->next->next;
		free(target);
		return 0;
	}
	return 1;
}
void printList(lista_akmwn* list){
	akmh* temp=list->arxi;

	printf("(");
	while(temp!=NULL){
		printf("%.7d",temp->nodeptr->keyName);
		temp=temp->next;
		if(temp!=NULL){
			printf(",");
		}
	}
	printf(")");
}
int 		printcycles(graphNode* current,graphNode*target,lista_akmwn* list){
	int flag=0;
	akmh* akmes=current->payments->arxi;
	
	while(akmes!=NULL){

		if(akmes->nodeptr==NULL){
			continue;
		}
		if(akmes->nodeptr==target){
			akmes=akmes->next;
			if(list->size<3){
				continue;
			}
			printList(list);
			flag+=1;
			printf("\n");
			continue;
		}
		if(akmes->nodeptr->check==1){
			akmes=akmes->next;
			continue;
		}
		akmes->nodeptr->check=1;
		addakmh(list,akmes->nodeptr);
		flag+=printcycles(akmes->nodeptr,target,list);
		removeakmh(list,akmes->nodeptr);

		akmes=akmes->next;
	}	
	return flag;
}


int 	path(graphNode*current,lista_akmwn* list,int leftover,double amount){
	int flag=0;
	if(leftover==0){
		printList(list);
		printf("= %f\n",amount);
		return 1;
	}
	akmh* akmes=current->payments->arxi;
	while(akmes!=NULL){
		if(akmes->nodeptr==NULL){
			return -1;
		}
		if(akmes->nodeptr->check==1){
			akmes=akmes->next;
			continue;
		}
		akmes->nodeptr->check=1;

		addakmh(list,akmes->nodeptr);
		amount+=akmes->poso;
		flag+=path(akmes->nodeptr,list,leftover-1,amount);
		amount-=akmes->poso;
		removeakmh(list,akmes->nodeptr);

		akmes->nodeptr->check=0;
		akmes=akmes->next;
	}	

	return flag;

}