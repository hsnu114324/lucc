#ifndef ARRAY_H
#define ARRAY_H

#include "Lib.h"


typedef struct {
	int size;    
	int count;   
	void **item; 
} Array;


typedef void (*FuncPtr1) (void *); 
typedef int (*FuncPtr2) (void *, void *); 

void ArrayTest(); 


Array* ArrayNew(int size);
void ArrayFree(Array *array, FuncPtr1 freeFuncPtr); 
void ArrayAdd(Array *array, void *item); 
void* ArrayGet(Array *array, int i); 
void* ArrayLast(Array *array); 
void ArrayPush(Array *array,void *item); 
void* ArrayPop(Array *array);  
void* ArrayPeek(Array *array); 
int ArrayFind(Array *array, void *data, FuncPtr2 fcmp); 
void ArrayEach(Array *array, FuncPtr1 f); 


void strPrint(void *data);  
void strPrintln(void *data);




typedef enum { KEEP_SPLITER, REMOVE_SPLITER } SplitMode;


extern Array* split(char *str, char *spliter, SplitMode mode); 

#endif
