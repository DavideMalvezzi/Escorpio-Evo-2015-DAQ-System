// List.h

#ifndef _LIST_h
#define _LIST_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

typedef struct Node{
	struct Node *pnext;
	unsigned char header;
	unsigned char* buffer;
	unsigned short bufferDim;
}Node;



class ListClass{
 private:
	 unsigned short count, maxDim;
	 Node *pfront, *pcurrent;
	 String name;

 public:
	 ListClass(String name, unsigned short maxDim);
	 Node* next();
	 Node* current();
	 Node* take();
	 Node* take(Node*);
	 Node* last();
	 void pop();
	 void pop(int);
	 void pop(Node*);
	 void insert(Node*);
	 void insert(unsigned char,unsigned char*, unsigned short);
	 void clear();
	 void reset();
	 unsigned short getCount();
	 unsigned short getMaxSize(){return maxDim;}
	 bool isFull(){return maxDim == count;}


	
};



#endif

