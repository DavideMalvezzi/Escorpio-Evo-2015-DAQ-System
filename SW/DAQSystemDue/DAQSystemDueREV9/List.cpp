#include "List.h"

ListClass::ListClass(String name, unsigned short maxDim){
	this->name = name;
	this->maxDim = maxDim;
	this->count = 0;
	pfront = pcurrent = NULL;
}

Node* ListClass::next(){
	if(pcurrent==NULL){
		pcurrent = pfront;
	}else{
		pcurrent = pcurrent->pnext;
	}
	return pcurrent;
}

Node* ListClass::current(){
	return pcurrent;
}

void ListClass::reset(){
	pcurrent = NULL;
}

Node* ListClass::take(){
	Node* ptemp = pfront;
	if(ptemp!=NULL){
		pfront = pfront->pnext;
		count--;
	}
	return ptemp;
}

Node* ListClass::take(Node*node){
	Node* ptemp = pfront;
	
	if(node == pfront){
		pfront = pfront->pnext;
		count--;
	}else{
		while(ptemp!=NULL && ptemp->pnext!=node){
			ptemp = ptemp->pnext;
		}

		if(ptemp!=NULL){
			ptemp->pnext = node->pnext;
			count--;
		}
	}

	if(node == pcurrent){
		//pcurrent = pcurrent->pnext;
		reset();
	}

	return node;
}

void ListClass::pop(){
	Node* ptemp = pfront;
	if(ptemp!=NULL){
		pfront = pfront->pnext;
		delete[] ptemp->buffer;
		delete ptemp;
		count--;
	}
}

void ListClass::pop(Node*node){
	Node* ptemp = pfront;
	
	if(node == pfront){
		pfront = pfront->pnext;
	}else{
		while(ptemp!=NULL && ptemp->pnext!=node){
			ptemp = ptemp->pnext;
		}
	}

	if(ptemp!=NULL){
		ptemp->pnext = node->pnext;
		delete[] node->buffer;
		delete node;
		count--;
	}
}

void ListClass::pop(int num){
	for(int i=0; i<num; i++){
		pop();
	}
}

void ListClass::insert(Node* node){
	if(count<maxDim){
		node->pnext = NULL;
		if(pfront == NULL){
			pfront = node;
		}else{
			last()->pnext = node;
		}
		count++;
	}else{
		Serial.print(name);Serial.println(" -> Lista piena");
	}
}

void ListClass::insert(unsigned char headerType, unsigned char* buffer, unsigned short bufferDim){
	Node *ptemp;
	if(count<maxDim){
		if(pfront == NULL){
			pfront = new Node;
			if(pfront){
				pfront->buffer = new unsigned char[bufferDim];
				if(pfront->buffer){
					memcpy(pfront->buffer,buffer,bufferDim);
					pfront->bufferDim = bufferDim;
					pfront->header = headerType;
					pfront->pnext = NULL;
					count++;
				}else{
					pfront = NULL;
					Serial.print(name);Serial.println("Memoria esaurita per nuovo messaggio");
				}
			}else{
				Serial.print(name);Serial.println("Memoria esaurita per nuovo nodo");
			}

		}else{
			ptemp = new Node;
			if(ptemp){
				ptemp->buffer = new unsigned char[bufferDim];
				if(ptemp->buffer){
					memcpy(ptemp->buffer,buffer,bufferDim);
					ptemp->bufferDim = bufferDim;
					ptemp->header = headerType;
					ptemp->pnext = NULL;
					last()->pnext = ptemp;
					count++;
				}else{
					Serial.print(name);Serial.println("Memoria esaurita per nuovo messaggio");
				}
			}else{
				Serial.print(name);Serial.println("Memoria esaurita");
			}
		}
	}else{
		Serial.print(name);Serial.println("Lista piena");
	}
}

void ListClass::clear(){
	while(count)pop();
	reset();
}

Node* ListClass::last(){
	Node *pa = pfront;

	if(pa != NULL){
		while(pa->pnext != NULL){
			pa = pa->pnext;
		}
	}

	return pa;
}

unsigned short ListClass::getCount(){
	return count;
}



