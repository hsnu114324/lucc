#ifndef PARSER_H
#define PARSER_H

#include "Scanner.h"
#include "Tree.h"
#include "Lib.h"
#include "SymTable.h"

typedef struct {                
	Array *nodeStack;       
	Array *blockStack;     
	PType  ptype;         
	Scanner *scanner;      
	SymTable *symTable;     
	Tree *atomLast, *op;
	char  spaces[MAX_LEN]; 
} Parser;                                                     

Tree *parse(char *text, char *asmFileName);
Parser *ParserNew(Scanner *scanner); 
Tree *ParserParse(Scanner *s, char *text, char *asmFileName);
void ParserFree(Parser *parser);          

Tree* parseProg(Scanner *s, char* asmFileName);     
Tree* parseBase(Scanner *s);     
Tree* parseStruct(Parser *p);   
Tree* parseMethod(Scanner *s, char* asmFileName);   
Tree* parseDecl(Scanner *s);     
Tree* parseIf(Scanner *s);       
Tree* parseFor(Scanner *s);      
Tree* parseWhile(Scanner *S);    
Tree* parseStmt(Scanner *s);     
Tree* parseBlock(Scanner *s);    
Tree* parseVar(Scanner *s);      
Tree* parseExp(Scanner *s);      
Tree* parseTerm(Scanner *s);     
Tree* parsePath(Scanner *s);     
Tree* parseAtom(Scanner *p);     
Tree* parseDecl(Scanner *s);     
Tree* parseParam(Parser *p);    
Tree* parseVarList(Scanner *s);  
Tree* parseExpList(Scanner *s);  
Tree* parseDeclList(Parser *p); 
Tree* parseParamList(Parser *p);
Tree* parseEType(Scanner *s);    
Tree* parseId(Parser *p);       

BOOL isMethod(Scanner *s);  
BOOL isDecl(Scanner *s);   
#define push(p, tag, SemType) sem=ObjNew(SemType, 1);Tree *node=push1(p, tag);node->sem=sem; 
Tree *push1(Parser *p, char* tag);   
Tree *pop(Parser *p, char* tag);     
BOOL isNext(Scanner *s, char *tags);  
char *next(Scanner *p, char *tags);   
char *token(Tree *node);             

void pushBlock(Parser *p, Symbol *sym);
#define popBlock(p) ArrayPop(p->blockStack) 
#define peekBlock(p) ArrayPeek(p->blockStack)  

#define SET_KEYWORDS "|if|else|for|while|return|def|int|byte|char|float|struct|"
#define SET_OP1 "|++|--|"
#define SET_OP2 "|+|-|*|/|%|^|&|<<|>>|==|!=|<=|>=|<|>|&&||||"
#define SET_BTYPE "|int|byte|char|float|"


typedef struct { } SemProg;
typedef struct {  } SemBase;
typedef struct { Tree *exp, *base1, *base2; } SemIf;
typedef struct { Tree *stmt1, *exp, *stmt2; } SemFor;
typedef struct { Tree *exp, *base; } SemWhile;
typedef struct { Tree *type, *id, *paramList, *block; Symbol *symMethod; } SemMethod; // /*int starCount; char *returnType;
typedef struct { Tree *id; Symbol *symStruct; } SemStruct;
typedef struct { Tree *id, *exp; PType *ptype; } SemVar; // int starCount, dimList[10], dimCount;
typedef struct { Tree *exp, *decl, *path, *op1, *expList; } SemStmt;
typedef struct { Tree *type, *varList; } SemDecl;
typedef struct { Tree *type, *var; } SemParam;
typedef struct { Tree *baseList; Symbol *symBlock; } SemBlock;
typedef struct { Tree *term1, *op, *term2;  PType ptype; } SemExp;
typedef struct { Tree *exp1, *op, *exp2, *path;  char *subTag; PType ptype; } SemTerm; // int int1; float float1; char *str1;
typedef struct { PType ptype; } SemPath;
typedef struct { Tree *id, *expList; char *subTag; PType ptype; } SemAtom;
typedef struct { Array *varList; } SemVarList;
typedef struct { Array *expList; } SemExpList;
typedef struct { Array *declList; } SemDeclList;
typedef struct { Array *paramList; } SemParamList;
typedef struct { } SemEType;

Symbol* SymbolLookup(Parser *p, char *name);
#define PTypeCopy(to, from) ObjCopy(to, from, PType)

#endif
