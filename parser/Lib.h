#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>

#define FALSE   0
#define TRUE    1
#define BYTE    unsigned char
#define BOOL    unsigned char
#define INT32   int
#define INT16   short
#define INT8    char
#define UINT32  unsigned int
#define UINT16  unsigned short
#define UINT8   unsigned char
#define MAX_LEN 512

#define min(x,y)         (x < y?x:y)    
#define max(x,y)         (x > y?x:y)    

void* memNew(int size);     
void  memFree(void *ptr);   
void  memCheck();           
#define ObjNew(type, count) memNew(count*sizeof(type)) 
#define ObjFree memFree     
#define ObjCopy(to, from, type) memcpy(to, from, sizeof(type)) 


char* strNew(char *str); 
#define strFree memFree  
char* strSub(char *str, int i, int len); 
#define strEqual(str1, str2) (strcmp(str1, str2)==0)
#define strMember(ch, set) (strchr(set, ch) != NULL)
char* strSubstr(char *substr, char *str, int i, int len);
BOOL  strHead(char *str, char *head);   
BOOL  strTail(char *str, char *tail);   
BOOL  strPartOf(char *token, char *set); 
int   strCountChar(char *str, char *charSet);
char* strReplace(char *str, char *fromSet, char to); 
char* strTrim(char *trimStr, char *str, char *set);  
char* strFill(char *toStr, char ch, int n); 
char* strToUpper(char *str); 
char* strToLower(char *str); 
#define strNoNull(s) (s==NULL?"":s) 

#ifdef DEBUG
#define ASSERT(cond)     if (!(cond)) ERROR()  
#else
#define ASSERT(cond)
#endif

void debug(const char *fmt, ...);  
#define ERROR()	do { \
		printf("ERROR~test\n");\
                  printf("ERROR => file:%s, line: %d\n", __FILE__, __LINE__);\
                  exit(1); \
                } while (1)

BYTE* fileToBytes(char *fileName, int *sizePtr);  
char* fileToStr(char *fileName);  

#endif

#define SPACE " \t\n\r\t"     
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" 
#define DIGIT "0123456789"    
#define SYMBOL "~!@#$%^&*()_+{}:<>?-=[]|\"\\;',./" 
