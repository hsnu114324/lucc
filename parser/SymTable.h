#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "Lib.h"
#include "HashTable.h"
#include "Tree.h"

typedef char SymType;
#define SymBlock  'B'
#define SymMethod 'M'
#define SymStruct 'S'
#define SymVar    'V'
#define SymBType  'T'

typedef struct Symbol { 
    struct Symbol *scope; 
    char    *name;  
    SymType symType; 
    void    *typePtr; 
    int     size;   
} Symbol;

#define DIM_MAX 5
typedef struct PType {
    Symbol *typeSym;
    int   starCount;  
    int   dim[DIM_MAX], dimCount; 
    int   offset; 
} PType;

#define PTR_SIZE 4
typedef struct BType {
    char temp; 
} BType;

typedef struct Block { 
    Array *vars;
} Block;

typedef struct Method {
    PType ret;
    Array *params;
    Symbol *block;
} Method;

typedef struct Struct {
    Array *fields;

} Struct;


typedef union Value {
    BYTE  bvalue;
    int   ivalue;
    float fvalue;
    void  *pvalue;
} Value;

#define SymTable HashTable

SymTable *SymTableNew();
Symbol *SymTablePutBType(SymTable *st, char *name, int size);
Symbol *SymTablePut(SymTable *table, Symbol *sym);
Symbol *SymTableGet(SymTable *table, void *scope, char *name);
void SymTableFree(SymTable *table);
void SymTableDebug(SymTable *table);

Symbol *SymNew(Symbol *scope, char *name, SymType symType);
void SymFree(Symbol *s);

Block *BlockNew();
void BlockFree(Block *block);

Method *MethodNew();
void MethodFree();

Struct *StructNew();
void StructFree();

PType *PTypeNew();
void PTypeFree(PType *pt);
char *PTypeToStr(char *str, PType *ptype);

BType *BTypeNew();
void BTypeFree(BType *b);

void SymDebug(Symbol *s);
void PTypeDebug(PType *pt);

Symbol *Global , *Int  , *Float  , *Char ; 

#endif
