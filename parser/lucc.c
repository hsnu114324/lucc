#include "Lib.h"
#include "Parser.h"

char *CINT  = "CINT";       
char *CFLOAT  = "CFLOAT";   
char *CSTR  = "CSTR";       
char *ID  = "ID";           
char *OP1  = "OP1";         

char *PROG  = "PROG";           
char *METHOD  = "METHOD";       
char *STRUCT  = "STRUCT";       
char *BLOCK  = "BLOCK";         
char *BASE  = "BASE";           
char *IF  = "IF";               
char *FOR  = "FOR";             
char *WHILE  = "WHILE";         
char *STMT  = "STMT";           
char *VAR  = "VAR";             
char *EXP  = "EXP";             
char *TERM  = "TERM";           
char *PATH  = "PATH";           
char *ATOM  = "ATOM";           
char *CALL  = "CALL";           
char *ARRAY_MEMBER  = "ARRAY_MEMBER"; 
char *DECL  = "DECL";           
char *PARAM  = "PARAM";         
char *TYPE  = "TYPE";           
char *DECL_LIST  = "DECL_LIST"; 
char *PARAM_LIST  = "PARAM_LIST";
char *EXP_LIST  = "EXP_LIST";   
char *VAR_LIST  = "VAR_LIST";   
char *BTYPE =  "BTYPE";         
char *ETYPE =  "ETYPE";         
char *GLOBAL =  "GLOBAL";       

typedef enum {
	kIF = 0,
	kELSE,
	kFOR,
	kWHILE,
	kRETURN,
	kINT,
	kBYTE,
	kCHAR,
	kFLOAT,
	kSTR,
	kSTRUCT,
	sLBRACE,
	sRBRACE,
	sLPAREN,
	sRPAREN,
	sLSQUARE,
	sRSQUARE,
	sSEMI,
	sCOMMA,
	sARROW,
	sDOT,
	sLT,
	sGT,
	sASSIGN,
	sLE,
	sGE,
	sEQ,
	sNE,
	sLAND,
	sLOR,
	sNOT,
	sADD,
	sSUB,
	sSTAR,
	sDIV,
	sMOD,
	sSHL,
	sSHR,
	sBAND,
	sBOR,
	sDPLUS,
	sDMINUS,
	kEND
		
} eTokenID ;


typedef struct {
	eTokenID TokenID;
	char *szToken;
} UARTCMD ;

volatile UARTCMD gTokenList[] = 
{
	{kIF,		"if"  } ,
	{kELSE,		"else"  } ,
	{kFOR,		"for"  } ,
	{kWHILE,	"while"  } ,
	{kRETURN,	"return"  } ,
	{kINT,		"int"  } ,
	{kBYTE,		"byte"  } ,
	{kCHAR,		"char"  } ,
	{kFLOAT,	"float"  } ,
	{kSTR,		"str"  } ,
	{kSTRUCT,	"struct"  } ,
	{sLBRACE,	"{"  } ,
	{sRBRACE,	"}"  } ,
	{sLPAREN,	"("  } ,
	{sRPAREN,	")"  } ,
	{sLSQUARE,	"]"  } ,
	{sRSQUARE,	"["  } ,
	{sSEMI,		";"  } ,
	{sCOMMA,	","  } ,
	{sARROW,	"->"  } ,
	{sDOT,		"."  } ,
	{sLT,		"<"  } ,
	{sGT,		">"  } ,
	{sASSIGN,	"="  } ,
	{sLE,		"<="  } ,
	{sGE,		">="  } ,
	{sEQ,		"=="  } ,
	{sNE,		"!="  } ,
	{sLAND,		"&&"  } ,
	{sLOR,		"||"  } ,
	{sNOT,		"!"  } ,
	{sADD,		"+"  } ,
	{sSUB,		"-"  } ,
	{sSTAR,		"*"  } ,
	{sDIV,		"/"  } ,
	{sMOD,		"%"  } ,
	{sSHL,		">>"  } ,
	{sSHR,		"<<"  } ,
	{sBAND,		"&"  } ,
	{sBOR,		"|"  } ,
	{sDPLUS,	"++"  } ,
	{sDMINUS,	"--"  } ,
	{kEND,		"_?END?_"  } 
};

char *volatile *gkEND = &gTokenList[kEND].szToken; 

#ifdef DEBUG
int memNewCount = 0; 
int memFreeCount=0; 
#endif

void debug(const char *fmt, ...) {
#ifdef DEBUG
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
#endif
}

#define ObjNew(type, count) memNew(count*sizeof(type)) 
#define ObjFree memFree     
#define ObjCopy(to, from, type) memcpy(to, from, sizeof(type)) 

#define strFree memFree  
#define strEqual(str1, str2) (strcmp(str1, str2)==0)
#define strMember(ch, set) (strchr(set, ch) != NULL)

#ifdef DEBUG
#define ASSERT(cond)     if (!(cond)) ERROR()  
#else
#define ASSERT(cond)
#endif

#define SPACE " \t\n\r\t"     
#define ALPHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" 
#define DIGIT "0123456789"    
#define SYMBOL "~!@#$%^&*()_+{}:<>?-=[]|\"\\;',./" 

char *m_itoa (int n)
 {
         char s[64];
         static char s2[64];
     int i, sign, j;

     if ((sign = n) < 0)  
         n = -n;          
     i = 0;
     do {       
         s[i++] = n % 10 + '0';   
     } while ((n /= 10) > 0);     
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';

     for (j = 0 ; j < i ; j++)
        s2[j] = s[i-j-1] ;

     s2[i]='\0' ;

     return s2 ;
 }


void* memNew(int size) {
        void *ptr=malloc(size); 
        ASSERT(ptr != NULL);    
        memset(ptr, 0, size);   
#ifdef DEBUG
        memNewCount++;          
#endif
        return ptr;             
}

void memFree(void *ptr) {
        ASSERT(ptr != NULL);    
    free(ptr);              
#ifdef DEBUG
        memFreeCount++;         
#endif
}


char *strNew(char *str) {
        char *rzStr = memNew(strlen(str)+1);
        strcpy(rzStr, str);
        return rzStr;
}

char* strSubstr(char *substr, char *str, int i, int len) {
    ASSERT(len >= 0);
        strncpy(substr, &str[i], len);
        substr[len]='\0';
        return substr;
}

char *strSub(char *str, int i, int len) {
        char *rzStr = memNew(len+1);
        strSubstr(rzStr, str, i, len);
        return rzStr;
}

BOOL strHead(char *str, char *head) {
        return (strncmp(str, head, strlen(head))==0);
}

BOOL strTail(char *str, char *tail) {
        int strLen = strlen(str), tailLen = strlen(tail);
        if (strLen < tailLen) return FALSE;
        return (strcmp(str+strLen-tailLen, tail)==0);
}

void strCut(char *str, char *spliter, char *head, char *tail) {
    int i, len = strlen(str);
    strcpy(head, str);
    for (i=0; i<len; i++) {
        if (strMember(str[i], spliter)) {
            if (head != NULL)
                strSubstr(head, str, 0, i);
            if (tail != NULL)
                strSubstr(tail, str, i+1, len-(i+1));
            break;
        }
    }
}

int strCountChar(char *str, char *charSet) {
        int i, count=0;
        for (i=0; i<strlen(str); i++)
                if (strMember(str[i], charSet))
                        count++;
        return count;
}

BOOL strPartOf(char *token, char *set) {
        ASSERT(token != NULL && set != NULL);
        ASSERT(strlen(token) < MAX_LEN);
        char ttoken[MAX_LEN+2];
        sprintf(ttoken, "|%s|", token);
        return (strstr(set, ttoken)!=NULL);
}

char* strTrim(char *trimStr, char *str, char *set) {
        char *start, *stop;
        for (start = str; strMember(*start, set); start++);
        for (stop = str+strlen(str)-1; stop > str && strMember(*stop, set); stop--);
        if (start <= stop) {
                strncpy(trimStr, start, stop-start+1);
                trimStr[stop-start+1]='\0';
        } else
                strcpy(trimStr, "");
        return trimStr;
}

char* strReplace(char *str, char *fromSet, char to) {
        int i;
        for (i=0; i<strlen(str); i++) {
                if (strMember(str[i], fromSet))
                        str[i] = to;
    }
        return str;
}

char* strFill(char *str, char ch, int n) {
        memset(str, ch, n);
        str[n] = '\0';
        return str;
}

char* strToUpper(char *str) {
        int i;
        for (i = 0; i<strlen(str); i++)
                str[i] = toupper(str[i]);
        return str;
}

char* strToLower(char *str) {
        int i;
        for (i = 0; i<strlen(str); i++)
                str[i] = tolower(str[i]);
        return str;
}


BYTE* fileToBytes(char *fileName, int *sizePtr) {
        FILE *file = fopen(fileName, "rb");    
        fseek(file, 0 , SEEK_END);             
        long size = ftell(file);               
        rewind(file);                          
        BYTE *buffer = (BYTE*) memNew(size+1); 
        fread (buffer,size,1,file);            
        fclose(file);                          
        *sizePtr = size;                       
        return buffer;                         
}


char* fileToStr(char *fileName) {
        int size;                                      
        BYTE *buffer = fileToBytes(fileName, &size);   
        buffer[size] = '\0';                           
        return (char*) buffer;                         
}


Array* ArrayNew(int size) {
        Array *array = ObjNew(Array, 1);
        array->count = 0;
        array->size = size;
        array->item = ObjNew(void*, array->size);
        return array;
}

void ArrayAdd(Array *array, void *item) {
        ASSERT(array->count <= array->size);
        if (array->count == array->size) {
                int newSize = array->size*2;
                void **newItems = ObjNew(void*, newSize);
                memcpy(newItems, array->item, array->size*sizeof(void*));
                ObjFree(array->item);
                array->item = newItems;
                array->size = newSize;
        }
        array->item[array->count++] = item;
}

int ArrayFind(Array *array, void *data, FuncPtr2 fcmp) {
        int i;
        for (i=0; i<array->count; i++)
                if (fcmp(array->item[i], data)==0)
                        return i;
        return -1;
}

void* ArrayLast(Array *array) {
        if (array->count > 0)
        return array->item[array->count-1];
    else
        return NULL;
}


void ArrayPush(Array *array, void *item) {
        ArrayAdd(array, item);
}


void* ArrayPop(Array *array) {
        ASSERT(array->count>0);
        void *top = ArrayLast(array);
        array->count--;
        return top;
}

void* ArrayPeek(Array *array) {
        return ArrayLast(array);
}

Tree* global_ptr;

void TreePrintLevel(Tree *tree, int level) { 
        
        ASSERT(tree != NULL);
          printf("address of tree->type=%s\n", tree->type);
        char spaces[MAX_LEN];
        ASSERT(level < MAX_LEN);
        strFill(spaces, ' ', level);
        if (tree->childs == NULL) {
        printf("%s+%s:%s:%s\n", spaces, tree->tag, token(tree), strNoNull(tree->type)); 
        } else {
        printf("%s+%s:%s\n", spaces, tree->tag, strNoNull(tree->type));
                int i;
                for (i=0; i<tree->childs->count; i++)
                        TreePrintLevel(tree->childs->item[i], level + 1);
                printf("%s-%s\n", spaces, tree->tag);
        }
}


void linklist_link(Tree *prev, Tree* next ){
	Tree* ptr = prev;
	while(ptr->rlink != NULL){ ptr = ptr->rlink; }
	ptr->rlink = next;
	next->llink = ptr;
}


char *tokenToTag(char *token) { 
    if (token[0] == '\"') 
                return CSTR; 
        else if (strMember(token[0], DIGIT)) {
                if (strMember('.', token))
                        return CFLOAT;
                else
                        return CINT;
        } else { 
        
        int i;

            for (i=0; gTokenList[i].TokenID != kEND; i++){
                if ( strEqual(token, gTokenList[i].szToken) ) 
                    return gTokenList[i].szToken;
            }

            if (strMember(token[0], ALPHA)) 
                   return ID; 
        else
            ERROR();
    }
}


Scanner* ScannerNew(char *pText) {
        Scanner *s = ObjNew(Scanner, 1);
        s->text = pText;
        s->len = strlen(pText);
        s->i = 0;
        s->line = 1;
    s->pos = 1;
      ScannerNext(s);
        return s;
}

char ch(Scanner *s) {
    return s->text[s->i];
}


void cnext(Scanner *s) {
    s->i++;s->pos++;
}

#define OP "+*-/%<=>!&|"        

void ScannerNext(Scanner *s) { 

	debug("s->i : %d", s->i);
	debug("s->text : %s\n", s->text);

        while (strMember(ch(s), SPACE)) { 
            if (ch(s)=='\n') {
            s->line++;
            s->pos = 1;
        }
                cnext(s);
    }
        if (s->i >= s->len) { 
            s->tag = gkEND; 
            s->token[0] = '\0'; 
            return;
    }
        char c = ch(s); 
        int begin = s->i; 
        if (c == '\"') { 
            
                cnext(s); 
                while (ch(s) != '\"') cnext(s); 
                cnext(s); 
        } else if (strMember(c, OP)) { 
            
                while (strMember(ch(s), OP)) cnext(s); 
        } else if (strMember(c, DIGIT)) { 
            
                while (strMember(ch(s), DIGIT)) cnext(s); 
                
                if (ch(s) == '.') cnext(s); 
                while (strMember(ch(s), DIGIT)) cnext(s); 
        } else if (strMember(c, ALPHA)) { 
            
                while (strMember(ch(s), ALPHA) || strMember(ch(s), DIGIT))
            cnext(s); 
        } else 
                cnext(s); 

    
        strSubstr(s->token, s->text, begin, (s->i) - begin);

        
        s->tag = tokenToTag(s->token);
}

void ScannerRestore(Scanner *s) {
    s->i = s->iSave;
    s->pos = s->posSave;
    s->line = s->lineSave;
    s->tag = s->tagSave;
    strcpy(s->token, s->tokenSave);
}

void ScannerStore(Scanner *s) {
    s->iSave = s->i;
    s->posSave = s->pos;
    s->lineSave = s->line;
    s->tagSave = s->tag;
    strcpy(s->tokenSave, s->token);
}


Parser *ParserNew(Scanner *scanner) {
	Parser *p = ObjNew(Parser, 1);
	p->scanner = scanner;
	ScannerNext(scanner); 
	return p;
}


char *next(Scanner *s, char *tags) { 

	char* s_token = malloc(sizeof(20));
	if (isNext(s, tags)) {            

		debug("s->tag = %s , s->token = %s \n", s->tag, s->token);
		strncpy(s_token, s->token, 20);

		ScannerNext(s);

		return s_token;
	}
	ERROR();
	return NULL;
}



BOOL isNext(Scanner *s, char *tags) { 

	char tTags[MAX_LEN+1];
	sprintf(tTags, "|%s|", tags);

	debug("s->tag: %s, tTags: %s\n",s->tag, tTags);
	if (strPartOf(s->tag, tTags)){
		return TRUE;
	}else{
		return FALSE;
	}
}

BOOL isMethod(Scanner *s) {
    BOOL rzFlag = TRUE;
    
    ScannerStore(s); 
    if (isNext(s, "int|byte|char|float|ID")) 
        ScannerNext(s); 
    else
        rzFlag=FALSE;
    while (isNext(s, "*")) ScannerNext(s); 
    if (isNext(s, ID))  
        ScannerNext(s); 
    else
        rzFlag=FALSE;
    if (!isNext(s, "(")) rzFlag=FALSE; 
    ScannerRestore(s); 
    return rzFlag;
}

char *token(Tree *node) {
    return (char*) node->sem;
}


int count = 1;

char* gen_label(){

	char *tmp_str;
	char* ttmp_str = malloc(sizeof(20));
	tmp_str = m_itoa(count);
	sprintf(ttmp_str,".L%s",tmp_str);
	debug("gen_label count:%d tmp_str:%s ttmp_str:%s\n",count,tmp_str,ttmp_str);
	count++;
	return ttmp_str;
}


Tree* parseEType(Scanner *s) {

	next(s, "int|byte|char|float|ID");  
	return NULL;
}


void printTree(Tree *tree ){
#ifdef DEBUG
	debug("%s %s %s %s %s \n", tree->op, tree->p1, tree->p2, tree->pTo, tree->pToo);
	if(tree->rlink !=NULL){
		debug("rlink->printTree\n");
		Tree *ptr = tree->rlink;
		printTree(ptr);
	}
#endif
}


void tree_fill(Tree* tree, char* op, char* p1, char* p2, char* pTo, char* pToo){

	strncpy(tree->op , op, 20);
	strncpy(tree->p1 , p1, 20);
	strncpy(tree->p2 , p2, 20);
	strncpy(tree->pTo , pTo, 20);
	strncpy(tree->pToo , pToo, 20);
}

void linklist_re_link(Tree *prev, Tree* next ){
	Tree* ptr = next;
	while(ptr->llink != NULL){
		ptr = ptr->llink;
	}
	prev->rlink = ptr;
	ptr->llink = prev;
}


Tree* parseExpList(Scanner *s) {

	char label_string_exp[20];
	Tree* exp = parseExp(s);
	debug("exp->string : %s\n",exp->string);
	Tree* exp2;
	Tree* exp_2 = (Tree *)malloc(sizeof(Tree));
	exp_2->rlink = NULL;
	int exp_2_use = 0;
	int exp2_use = 0;
	int exp2_2_use = 0;

	if(exp->num == 0){
		debug("parseExpList exp->num == 0\n");

		tree_fill( exp,"op_arg",exp->string,"","","" );
	}else{
		debug("parseExpList exp->num != 0\n");

		char* tmp_str;
		tmp_str = m_itoa(count++);
		sprintf(label_string_exp,".L%s",tmp_str);		

		debug("exp->string : %s\n",exp->string);
		tree_fill( exp_2,label_string_exp,"=",exp->string,"","" );
		tree_fill( exp,"op_arg",label_string_exp,"","","" );
		debug("printTree(exp_2):\n");
		printTree(exp_2);
		exp_2_use = 1;		
	}

	Tree* exp2_2;
	while (isNext(s, ",")) {
		next(s, ",");
		exp2 = parseExp(s);
		debug("parseExpList~~22\n");
		debug("exp2->string : %s\n",exp2->string);

		if(exp2->num == 0){
			tree_fill( exp2,"op_arg",exp2->string,"","","" );
			exp2_use = 1;

		}else{
			debug("parseExpList exp2->num != 0\n");

			char label_string_exp[20];
			char* tmp_str;
			tmp_str = m_itoa(count++);
			sprintf(label_string_exp,".L%s",tmp_str);		

			exp2_2 = (Tree *)malloc(sizeof(Tree));
			exp2_2->rlink = NULL;

			tree_fill( exp2_2,label_string_exp,"=",exp2->string,"","" );
			tree_fill( exp2,"op_arg",label_string_exp,"","","" );


			linklist_link(exp_2,exp2_2);
			exp2_2_use = 1;

		}


		linklist_link(exp,exp2);
	}


	global_ptr = (Tree *)malloc(sizeof(Tree));
	global_ptr->rlink = NULL;
	tree_fill( global_ptr,label_string_exp,"=",exp->string,"","" );
	//global_ptr = exp_2;

	debug("exp2_2_use : %d, exp_2_use : %d, exp2_use: %d\n",exp2_2_use, exp_2_use, exp2_use);
	if(exp2_2_use){
		debug("printTree(exp2_2)~1\n");
		printTree(exp2_2);
		debug("printTree(exp2_2)~2\n");
		return exp2_2;
	}else if (exp_2_use){
		debug("printTree(exp)~1\n");
		printTree(exp);
		debug("printTree(exp)~2\n");
		return exp;
	}else if (exp2_use){
		debug("printTree(exp2)~1\n");
		printTree(exp2);
		debug("printTree(exp2)~2\n");
		return exp2;
	}else{
		debug("printTree(exp)~3\n");
		printTree(exp);
		debug("printTree(exp)~4\n");
		return exp;
	}

}




Tree* parseAtom(Scanner *s) {

	Tree* Atom = ( Tree *) malloc(sizeof( Tree));
	Atom->rlink = NULL;
	char* ttoken = next(s, ID);

	sprintf(Atom->string ,"%s", ttoken); 

	Tree* explist = (Tree*)malloc(sizeof(Tree));
	explist->rlink = NULL;

	if (isNext(s, "(")) { 

		next(s, "(");
		if (!isNext(s, ")")){

			explist = parseExpList(s);
			tree_fill( Atom,"op_call",Atom->string,"","","" );
			linklist_link(explist,Atom);
		}

		next(s, ")");
		printTree(explist);
		return explist;
	} else { 
		Atom->num = 0;	
		return Atom;

	}

}



Tree* parsePath(Scanner *s) {

	Tree *Atom  = parseAtom(s);
	debug("Atom->num : %d\n",Atom->num);

	if(Atom->num == 0){

	}
	while (isNext(s, ".|->")) { 

		next(s, ".|->");

		Tree *Atom_2  = parseAtom(s);
		linklist_link(Atom,Atom_2);
	}
	printTree(Atom);
	return Atom;
}


Tree* parseTerm(Scanner *s) {
	Tree* term = (Tree*)malloc(sizeof(Tree));
	term->rlink = NULL;

	if (isNext(s, "(")) {

		next(s, "(");
		Tree *exp1 = parseExp(s);
		if (!isNext(s, ")")) {
			next(s, SET_OP2);
			Tree *exp2 = parseExp(s);
			linklist_link(exp1, exp2);
		}
		next(s, ")");


	} else if (isNext(s, "CINT")) {

		char* ttoken = next(s, "CINT");
		strncpy(term->string, ttoken, 20);
		term->num = 0;

	} else if (isNext(s, "CSTR")) {

		char* ttoken =         next(s, "CSTR");
		strncpy(term->string, ttoken, 20);
		term->num = count++;


	} else if (isNext(s, "CFLOAT")) {

		next(s, "CFLOAT");

	} else {

		Tree *path = parsePath(s);
		path->num = 0;



		return path;
	}
	debug("Oh! Oh!\n");

	return term;
}

char p2[20];
char pTo[20];
char pToo[20];

Tree* term2;

Tree* parseExp(Scanner *s) {

	int term_use = 0;
	int term2_use = 0;

	Tree* label_exp = ( Tree *)malloc(sizeof( Tree));
	label_exp->rlink = NULL;

	char label_string_exp[20];
	char* tmp_str;

	tmp_str = m_itoa(count++);

	sprintf(label_string_exp,".L%s",tmp_str);		

	tree_fill( label_exp,label_string_exp,"","","","" );
	Tree* exp = ( Tree *)malloc(sizeof( Tree));
	exp->rlink = NULL;
	Tree *term  = parseTerm(s);

	if(term->num == 0){

		if(term->string[0] != '\0'){
			strncpy(exp->p2, term->string, 20);
		}
		term->num = 0;
	}else{

		char *tmp_str;

		tmp_str = m_itoa(term->num);
		sprintf(p2,".L%s",tmp_str);

		term_use = 1;
		term->num = count++;
	}

	char *ttoken = malloc(sizeof(char));
	strncpy(ttoken, "",1);

	if (isNext(s, SET_OP2)) { 
		char L_str[20];
		char* tmp_str;

		tmp_str = m_itoa(count++);

		sprintf(L_str,".L%s",tmp_str);		

		sprintf(exp->op,"%s",L_str);
		sprintf(exp->p1,"=");
		exp->num = count-1;

		ttoken= next(s, SET_OP2);


		sprintf(exp->pTo,"%s",ttoken);

		term2  = parseTerm(s);

		if(term2->num == 0){

			sprintf(exp->pToo ,"%s", term2->string);
		}else{

			char *tmp_str;
			char ttmp_str[20];
			tmp_str = m_itoa(term2->num);

			sprintf(ttmp_str,".L%s",tmp_str);		

			memcpy(term2->op, "L_str", 20);

			printTree(term2);

			sprintf(pToo,"%s", ttmp_str);
			term2_use = 1;
		}

		debug("term2_use : %d, term_use : %d\n",term2_use, term_use);
		if(term_use == 1){
			linklist_link(term,exp);
			if(term2_use == 1){

				linklist_link(term2,term);
				return term2;
			}else{

				return term;
			}
		}else{
			if(term2_use == 1){
				linklist_link(term2,exp);
				return exp;

			}else{

				return exp;	
			}
		}

	}else{
		debug("!op_2 \n");
		return term;
	}

	debug("term2_use : %d, term_use : %d\n",term2_use, term_use);
	if(term2_use == 1){
		return term2;
	}else if(term_use == 1){
		return term;
	}
	debug("???...\n");

	return exp;

}

Tree* parseVar(Scanner *s) {

	char* ttoken;

	ttoken = next(s, ID); 

	Tree* exp;
	if (isNext(s, "=")) { 
		next(s, "=");

		exp = parseExp(s);
	}
	tree_fill(exp, "op_dec",ttoken,exp->string,"","");
	printTree(exp);

	return exp;

}

Tree* parseVarList(Scanner *s) {

	Tree* var = parseVar(s);
	Tree* var2;
	while (isNext(s, ",")) {
		next(s, ",");
		var2 = parseVar(s);
		linklist_link(var,var2);
	}
	return var;

}

Tree* parseDecl(Scanner *s) {

	parseEType(s); 

	Tree* varlist = parseVarList(s); 
	return varlist;

}

Tree* parseFor(Scanner *s) {
      
        next(s, "for");                   
        next(s, "(");                     

	Tree* label_stmt1 = ( Tree *)malloc(sizeof( Tree));
	label_stmt1->rlink = NULL;

	char label_string_stmt1[20];
	strncpy(label_string_stmt1, gen_label(), 20);

	char label_string_stmt1_title[20];
	sprintf(label_string_stmt1_title,"%s:",label_string_stmt1);
	tree_fill( label_stmt1,label_string_stmt1_title,"","","","" );

	Tree* stmt1;

	if (!isNext(s, ";")) {
        	stmt1 = parseStmt(s);        
	}else{
		
	}
	
	char* label_no_str = gen_label();
	char label_no_str_title[20];
	sprintf(label_no_str_title,"%s:",label_no_str);

	Tree* label_no = ( Tree *)malloc(sizeof( Tree ));
	label_no->rlink = NULL;

	char* label_yes_str = gen_label();
	Tree* label_yes = ( Tree *)malloc(sizeof( Tree ));
	label_yes->rlink = NULL;
	
	Tree* goto_yes = ( Tree *)malloc(sizeof( Tree ));
	goto_yes->rlink = NULL;
	Tree* expr_2 = ( Tree *)malloc(sizeof( Tree ));
	expr_2->rlink = NULL;
	Tree* expr_2_dec = ( Tree *)malloc(sizeof( Tree ));
	expr_2_dec->rlink = NULL;
	Tree* exp_pre = ( Tree *)malloc(sizeof( Tree ));
	exp_pre->rlink = NULL;
        next(s, ";");                     
	if (!isNext(s, ";")) {

        	Tree* exp = parseExp(s);           

		printTree(exp);

		int num_exp = exp->num;
		char* tmp_exp = m_itoa(num_exp);
		char str_exp_title[20];
		char str_exp[20];
		sprintf(str_exp_title, ".L%s", tmp_exp);
		sprintf(str_exp, "!.L%s", tmp_exp);
		
		tree_fill( expr_2,"op_if(",str_exp,")goto",label_no_str,"" );
		
		tree_fill( exp_pre,"op_dec",str_exp_title,"","","");
		tree_fill( label_yes, label_yes_str,"","","",""  );
		tree_fill( goto_yes, "op_goto",label_yes_str,"","",""  );
		linklist_link(label_stmt1,exp_pre);
		linklist_link(exp_pre,exp);
		
		linklist_link(exp,expr_2);
		
	}else{
		
	}

	char* label_string_stmt3 = gen_label();

	Tree* goto_for_begin = ( Tree *)malloc(sizeof( Tree));
	goto_for_begin->rlink = NULL;
	Tree* stmt3;
        next(s, ";");                     
	if (!isNext(s, ")")) {

        	stmt3 = parseStmt(s);        

		Tree* label_stmt3 = ( Tree *)malloc(sizeof( Tree));
		label_stmt3->rlink = NULL;
		tree_fill( label_stmt3,label_string_stmt3,"","","","" );

		tree_fill( goto_for_begin,"op_goto",label_string_stmt1,"","","" );
		linklist_link(stmt3,goto_for_begin);
		printTree(goto_yes);

	}else{
		
	}

        next(s, ")");                     
        Tree * base = parseBase(s);                     
	
	tree_fill( label_yes,label_yes_str,"","","","" );
	tree_fill( label_no,label_no_str_title,"","","","" );

	Tree* goto_label_stmt3 = ( Tree *)malloc(sizeof( Tree));
	goto_label_stmt3->rlink = NULL;
	tree_fill( goto_label_stmt3,"op_goto",label_string_stmt3,"","","" );
	
	linklist_link(expr_2,base);
	linklist_link(base,stmt3);
	linklist_link(goto_for_begin,label_no);

	printTree(stmt1);
	
	return label_stmt1;
}


Tree* parseIf(Scanner *s) {

	next(s, "if");                     
	next(s, "(");                      

	Tree* exp = parseExp(s);
	next(s, ")");                      

	char* label_no_str = gen_label();
	char label_no_str_title[20];
	sprintf(label_no_str_title,"%s:",label_no_str);
	Tree* label_no = (Tree *)malloc(sizeof(Tree));
	label_no->rlink = NULL;
	tree_fill(label_no, label_no_str_title, "","","","");

	int num_exp = exp->num;
	char* tmp_exp = m_itoa(num_exp);
	char str_exp[20];
	char str_exp_dec[20];
	sprintf(str_exp, "!.L%s", tmp_exp);
	sprintf(str_exp_dec, ".L%s", tmp_exp);
	Tree* expr2_dec = (Tree *)malloc(sizeof(Tree));
	expr2_dec->rlink = NULL;
	Tree* expr2 = (Tree *)malloc(sizeof(Tree));
	expr2->rlink = NULL;
	tree_fill( expr2_dec, "op_dec", str_exp_dec, "", "", "" );
	tree_fill( expr2, "op_if(", str_exp, ")goto", label_no_str, "" );
	char* label_out_str = gen_label();
	char label_out_str_title[20];
	sprintf(label_out_str_title,"%s:",label_out_str);
	Tree* label_out = (Tree *)malloc(sizeof(Tree));
	label_out->rlink = NULL;
	tree_fill(label_out, label_out_str_title, "","","","");
	Tree* goto_out = (Tree *)malloc(sizeof(Tree));
	goto_out->rlink = NULL;
	tree_fill(goto_out, "op_goto",label_out_str,"","","");

	Tree* base = parseBase(s);

	linklist_link(expr2_dec,exp);
	linklist_link(exp,expr2);
	linklist_link(expr2,base);
	linklist_link(base,goto_out);

	linklist_link(goto_out,label_no);

	if (isNext(s, "else")) {           
		next(s, "else");               

		Tree* base2 = parseBase(s);

		linklist_link(label_no,base2);
		linklist_link(base2,label_out);

		printTree(expr2_dec);

		return expr2_dec;

        }

	linklist_link(label_no,label_out);

printTree(exp);

	return exp;
        
}


BOOL isDecl(Scanner *s) {
	BOOL rzFlag = TRUE;

	if (isNext(s, "int|byte|char|float")){

	}else{
		rzFlag=FALSE;
	}
	return rzFlag;
}


Tree* parseWhile(Scanner *s) {
        
        next(s, "while");                      
        next(s, "(");                          
        
	Tree* exp = parseExp(s);
        next(s, ")");                          

	Tree* label_begin = (Tree*)malloc(sizeof(Tree));
	label_begin->rlink = NULL;
	char label_begin_str[20];
	strncpy(label_begin_str, gen_label(), 20);
	tree_fill(label_begin, label_begin_str, "","","","");

	Tree* goto_begin = (Tree*)malloc(sizeof( Tree));
	goto_begin->rlink = NULL;
	tree_fill(goto_begin, "op_goto", label_begin_str,"","","");

        char* label_no_str = gen_label();
        Tree* label_no = (Tree *)malloc(sizeof(Tree));
	label_no->rlink = NULL;
        tree_fill(label_no, label_no_str, "","","","");

	int num_exp = exp->num;
	char* tmp_exp = m_itoa(num_exp);
	char str_exp[20];
	sprintf(str_exp, "!.L%s", tmp_exp);
	Tree* expr2 = (Tree *)malloc(sizeof(Tree));
	expr2->rlink = NULL;
	tree_fill(expr2, "op_if(", str_exp, ")goto", label_no_str, "");

	printTree(label_begin);
	Tree* base = parseBase(s);
	printTree(base);

	linklist_link(label_begin,exp);
	linklist_link(exp,expr2);
	linklist_link(expr2,base);
	linklist_link(base,goto_begin);
	linklist_link(goto_begin,label_no);

	printTree(label_begin);

	return label_begin;
        
}


Tree* parseStmt(Scanner *s) {

	if (isNext(s, "return")) { 
		next(s, "return");

	} else if (isDecl(s)) { 
		debug("parseStmt DECL s->tag : %s\n", s->tag);
		Tree *decl = parseDecl(s); 
		return decl;
	} else { 
		debug("parseStmt PATH s->tag : %s\n", s->tag);
		Tree *path = parsePath(s); 
		debug("path->string : %s\n",path->string);

		if (isNext(s, "=")) { 
			next(s, "=");
			Tree *exp2 = parseExp(s);
			if(exp2->num == 0){
				tree_fill( path,path->string,"=",exp2->string,"","" );

				printTree(path);
				return path;
			}else{

				char label_string_exp[20];
				char* tmp_str;
				tmp_str = m_itoa(exp2->num);
				sprintf(label_string_exp,".L%s",tmp_str);		

				Tree* exp2_pre = (Tree *)malloc(sizeof(Tree));
				exp2_pre->rlink = NULL;
				tree_fill(exp2_pre,"op_dec",label_string_exp,"","","");


				tree_fill( path,path->string,"=",label_string_exp,"","" );
				linklist_link(exp2,path);
				linklist_link(exp2_pre,exp2);
				printTree(exp2_pre);
				return exp2_pre;
			}
		} else if (isNext(s, SET_OP1)) { 
			char* ttoken = next(s, SET_OP1);
			char op1_par[20];
			if(!strcmp(ttoken,"++")){
				strncpy(op1_par, "+", 20);
			}else if(!strcmp(ttoken,"--")){ 
				strncpy(op1_par, "-", 20);
			}else{
				debug("Error~OP1");
			}

			if(path->num == 0){
				debug("op1_1\n");
				tree_fill( path,path->string,"=",path->string,op1_par,"1" );
				return path;
			}else{

				debug("op1_2\n");
			}
			debug("op1_3\n");


		} else if (isNext(s, ";")) { 
			debug("parseStmt \";\" s->tag : %s\n", s->tag);

			printTree(path);

			return path;
		} else{
			debug("parseStmt ERROR s->tag : %s\n", s->tag);
			ERROR();
		}
		debug("parseStmt~Error\n");
	}
	ERROR();
	return NULL;

}

Tree* parseBase(Scanner *s) { 

	if (isNext(s, "if")){ 
		Tree* If = parseIf(s); 
		return If;
	}else if (isNext(s, "for")){ 
		Tree* For = parseFor(s); 
		printTree(For);
		return For;
	}else if (isNext(s, "while")) 
		parseWhile(s); 
	else if (isNext(s, "{")){ 

		Tree* block = parseBlock(s); 
		debug("parseBase~3\n");
		printTree(block);
		return block;
	}
	else { 
		Tree* stmt =       parseStmt(s); 
		next(s, ";");                     
		debug("parseBase~2\n");
		printTree(stmt);
		return stmt;
	}
	ERROR();
	return NULL;

}


void rewrite(char *asmFileName, char* op, char* p1, char* p2, char* pTo, char* pToo){
	FILE *asmFile_fp;

	if((asmFile_fp = fopen(asmFileName,"at+")) == NULL) 
	{
		printf("file open error!");
		return ;
	}
	char *realOp = op;
	if (strEqual(op, "LD" ))
		realOp = "LDI";

	fprintf(asmFile_fp, "%s %s %s %s %s\n", realOp, p1, p2, pTo, pToo);
	debug( "rewrite: %-4s %-4s %-4s %-4s %-4s\n", realOp, p1, p2, pTo,pToo);
	fclose(asmFile_fp);
}

int generate_asm(Tree* tree,char *asmFileName){

        rewrite(asmFileName, tree->op, tree->p1, tree->p2, tree->pTo, tree->pToo);
        if(tree->rlink !=NULL){
                
                Tree *ptr = tree->rlink;
                generate_asm(ptr, asmFileName);
        }
	return 0;

}



Tree* parseBlock(Scanner *s) {

	next(s, "{"); 
	Tree* base = parseBase(s); 
	Tree* base2; 

	while (!isNext(s, "}")){
		base2 = parseBase(s);
		linklist_link(base,base2);	

	}
	next(s, "}");
	printTree(base);

	return base; 

}


Tree* parseMethod(Scanner *s, char* asmFileName) {

	parseEType(s);

	next(s, ID);

	next(s, "(");
	if (!isNext(s, ")")){ 

	}else{

	}
	next(s, ")");

	Tree* block = parseBlock(s); 
	printTree(block);

	return block;
}

Tree *parseProg(Scanner *s, char* asmFileName) { 

	Tree* result;

	if (isMethod(s)) { 
		result =       parseMethod(s, asmFileName);
		debug("parseProg~~End\n");
	}
	printTree(result);
	debug("global_ptr0\n");
	//return result;
	linklist_link(global_ptr,result);
	debug("global_ptr1\n");
	printTree(global_ptr);
	debug("global_ptr2\n");
	return global_ptr;
}

Tree *ParserParse(Scanner *s, char *text, char* asmFileName) { 
        Tree *tree = parseProg(s, asmFileName); 
       
        return tree;
}



Tree *parse(char *text, char* asmFileName) {  
        Scanner *scanner = ScannerNew(text);       

        Tree *tree = ParserParse(scanner, text, asmFileName);         

        return tree;                               
}

void compile(char *cFile, char *asmFile){
        debug("compile file:%s\n", cFile);
        char *cText = fileToStr(cFile);                    

	Tree *tree = parse(cText, asmFile);           

	generate_asm(tree,asmFile);
}


int main(int argc, char *argv[]){

        char cFile1[]="test.c1", *cFile=cFile1;                       
        char asmFile1[]="test.asm1", *asmFile=asmFile1;               

	if(argc == 3){
		cFile = argv[1]; asmFile = argv[2];
	}else{
		printf("usage: ./lucc test.c test.ir \n");
		return 0;
	}

	compile(cFile, asmFile);

        return 0;
}




