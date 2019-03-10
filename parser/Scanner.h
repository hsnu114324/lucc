#ifndef SCANNER_H
#define SCANNER_H

#include "Lib.h"

typedef struct {           
	char *text;            
	int len;               
	
	int i, iSave;          
	int line, lineSave;    
	int pos, posSave;      
	char *tag, *tagSave;   
	char token[100], tokenSave[100]; 
} Scanner;

void ScannerTest(char *fileName);   
Scanner* ScannerNew(char *pText);   
void ScannerFree(Scanner *s);       
void ScannerStore(Scanner *s);      
void ScannerRestore(Scanner *s);    
BOOL ScannerIsNext(Scanner *s, char *pTags); 
void ScannerNext(Scanner *s);       
char ch(Scanner *s);                
void cnext(Scanner *s);             
char *tokenToTag(char *token);      


#define DEF(var, str) extern char var[];

#undef DEF

#endif
