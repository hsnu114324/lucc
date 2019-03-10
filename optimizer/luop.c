#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 512

#define ASSERT(cond)     if (!(cond)) ERROR()  

#define ERROR() do { \
                  printf("ERROR => file:%s, line: %d\n", __FILE__, __LINE__);\
                  exit(1); \
                } while (1)

void debug(const char *fmt, ...) {
#ifdef DEBUG
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
#endif
}

unsigned char strPartOf(char *token, char *set) {
        ASSERT(token != NULL && set != NULL);
        ASSERT(strlen(token) < MAX_LEN);
        char ttoken[MAX_LEN+2];
        sprintf(ttoken, "|%s|", token);
        return (strstr(set, ttoken)!=NULL);
}

unsigned char strHead(char *str, char *head) {
        return (strncmp(str, head, strlen(head))==0);
}

typedef int bool;
#define true (1)
#define false (0)

typedef enum IR_Type {
    IR_NULL,
    
    IR_NOP,
    IR_LABEL,
    IR_FUNC,

    
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_ADDR,     
    IR_DEREF_R,  
    IR_DEREF_L,  

    
    IR_JMP,
    IR_BEQ,
    IR_BLT,
    IR_BLE,
    IR_BGT,
    IR_BGE,
    IR_BNE,
    IR_RET,

    
    IR_DEC,

    
    IR_ARG,      
    IR_CALL,
    IR_PARAM,    

    
    IR_READ,
    IR_WRITE,
    IR_WRITEC,

    
    IR_GLOBAL,

    IR_GOTO,

    NR_IR_TYPE
} IR_Type;


typedef struct Block {
    int index;
    int start;
    int end;
    union {
        struct {
            int follow;
            int branch;
        };
        int next[2];
    };
    int use_size;
    int def_size;
    int usenum;
    int defnum;
    int use;
    int** def;
    int unuse;
    int lead;
    int goto_param;
    int if_jump_in;
    int if_jump_out;
} Block;


typedef struct {
	char op[32];
	char p1[32];
	char p2[32];
	char pTo[32];
	char pToo[32];
	IR_Type type;
	int block;
	int unuse;
	int use;
} IR;

typedef struct{
	int size;
	int count;
	IR **item;
} IR_Array;

IR_Array array;

#define strMember(ch, set) (strchr(set, ch) != NULL)
#define strEqual(str1, str2) (strcmp(str1, str2)==0)
#define DIGIT "0123456789"    
#define MAX_LINE 4096

static IR instr_buffer[MAX_LINE];

int nr_instr = 0 ;

static Block blk_buf[MAX_LINE];
static int nr_blk;


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

	fclose(asmFile_fp);
}


int is_branch(IR *pIR){

    return IR_BEQ <= pIR->type && pIR->type <= IR_BNE;
}


bool can_jump(IR *pIR){

	debug("pIR->type = %d\n", pIR->type);

	if (pIR->type == IR_JMP) {
		return true;
	} else {
		return false;
	}
}


static bool is_leader(IR *instr[], int ir_idx){

	int result = 0;
	debug("instr[ir_idx]->op = %s\n",instr[ir_idx]->op);
	debug("instr[ir_idx]->type = %d\n",instr[ir_idx]->type);

	if(ir_idx == 0){
		result = 1;
	}else if(instr[ir_idx]->type == IR_LABEL){
		result = 1;
	}else if(instr[ir_idx]->type == IR_FUNC){
		result = 1;
	}else if(instr[ir_idx-1]->type == IR_JMP){
		result = 1;
	}else if(instr[ir_idx-1]->type == IR_GOTO){
		result = 1;
	}
	return result;
}


int block_partition(Block block[], IR *instr[], int n){

	int count = 0;
	int i = 0;
	for ( i = 0; i < n; i++) {
		debug("i = %d \n",i);
		block[count].unuse = 0;
		debug("instr[%d]->op = %s \n",i, instr[i]->op);
		if (is_leader(instr, i)) {

			if(instr[i]->type == IR_LABEL){

				if(strHead(instr[i]->op, ".")){

					int len = strlen(instr[i]->op);
					debug("instr[%d]->op, len = %d\n",i,len);
					char dot_check[10];
					dot_check[0]='\0';

					strncpy(dot_check, instr[i]->op+2, len-3);
					dot_check[len-3]='\0';

					debug("array.item[%d] dot_check : %s\n",i,dot_check);

					blk_buf[count].goto_param=atoi(dot_check);

					debug("blk_buf[%d].goto_param : %d\n",count,blk_buf[count].goto_param);
				}
			}
			debug("is_leader! i=%d \n",i);
			block[count].start = i;
			block[count].index = count;
			if (count > 0) {
				block[count - 1].end = i;
			}
			count++;
		}

		instr[i]->block = count - 1;
	}
	block[count - 1].end = n;
	return count;
}


char *m_itoa (int n){

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


int atoi(register const char * s){

	register int	a;
	register unsigned char	sign;

	while(*s == ' ' || *s == '\t')
		s++;
	a = 0;
	sign = 0;
	if(*s == '-') {
		sign++;
		s++;
	} else if(*s == '+')
		s++;
	while(isdigit(*s))
		a = a*10 + (*s++ - '0');
	if(sign)
		return -a;
	return a;
}


void constfold(){

	int i = 0;
	int tmp;
	int tmp_p2;
	int tmp_pToo;
		
	for(i = 0; i < nr_instr; i++){
		
	if(strPartOf(array.item[i]->pTo, "|+|-|*|/|>=|>|==|!=|<|<=|")){
		
	if(strMember(array.item[i]->p2[0], DIGIT) || array.item[i]->p2[0]=='-'){
		
	if(strMember(array.item[i]->pToo[0], DIGIT) || array.item[i]->pToo[0]=='-'){
		tmp_p2 = atoi(array.item[i]->p2);
		
		tmp_pToo = atoi(array.item[i]->pToo);

	if(array.item[i]->type == IR_ADD){
		tmp = atoi(array.item[i]->p2) + atoi(array.item[i]->pToo);
		debug("%s + %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_SUB){
		tmp = atoi(array.item[i]->p2) - atoi(array.item[i]->pToo);
		debug("%s - %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_MUL){
		tmp = atoi(array.item[i]->p2) * atoi(array.item[i]->pToo);
		debug("%s * %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_DIV){
		tmp = atoi(array.item[i]->p2) / atoi(array.item[i]->pToo);
		debug("%s / %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_BGT){
		tmp = atoi(array.item[i]->p2) > atoi(array.item[i]->pToo);
		debug("%s > %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_BGE){
		tmp = atoi(array.item[i]->p2) >= atoi(array.item[i]->pToo);
		debug("%s >= %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_BEQ){
		tmp = atoi(array.item[i]->p2) == atoi(array.item[i]->pToo);
		debug("%s == %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_BNE){
		tmp = atoi(array.item[i]->p2) != atoi(array.item[i]->pToo);
		debug("%s != %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_BLE){
		tmp = atoi(array.item[i]->p2) <= atoi(array.item[i]->pToo);
		debug("%s <= %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}else if(array.item[i]->type == IR_BLT){
		tmp = atoi(array.item[i]->p2) < atoi(array.item[i]->pToo);
		debug("%s < %s = ", array.item[i]->p2, array.item[i]->pToo ); 
		debug("tmp : %d\n", tmp);
	}
		
		debug("array.item[%d]->type = %d op = %s\n",i,array.item[i]->type, array.item[i]->op);
		char *tmp_str;
		tmp_str = m_itoa(tmp);
		debug("tmp_str = %s\n",tmp_str);
		strncpy(array.item[i]->p2, m_itoa(tmp), 32 );
		debug("array.item[%d]->p2 = %s\n",i,array.item[i]->p2);
		strncpy(array.item[i]->pTo, "", 32 );
		strncpy(array.item[i]->pToo, "", 32 );
		array.item[i]->type = IR_ASSIGN;
	}	
    	}
	}
	}
}




void add2def(int var_line,int blk)	
{
	int i = 0;
	int j = 0;
	int check_num = 0;
	int defnum = 0;

	debug("blk_buf[%d].defnum = %d\n",blk,blk_buf[blk].defnum);

	defnum = blk_buf[blk].defnum;

	int blk_lead = blk_buf[blk].lead;
	for (i=blk; i>=blk_lead; i--){
		if(blk_buf[i].unuse == 1) continue;
		for (j=blk_buf[i].defnum-1;j>=0;j--){
			int def_line = blk_buf[i].def[j];
			debug("blk_buf[%d].def[%d] = def_line : %d\n", i, j, def_line);
			debug("def array.item[%d]->op : %s\n",def_line,array.item[def_line]->op);
			debug("var array.item[%d]->op : %s\n",var_line,array.item[var_line]->op);
			if ( !strcmp( array.item[def_line]->op, array.item[var_line]->op ) ){

				if(array.item[def_line]->use == 0){
					array.item[def_line]->unuse=1;
				}else{
					break;
				}
			}
		}
	}	

	blk_buf[blk].def[defnum] = var_line;
	blk_buf[blk].defnum++;
	debug("var_line : %d\n",var_line);
}


void constprop(){

	int i = 0;
	int j = 0;
	int k = 0;
	int p2_find = 0;
	int pToo_find = 0;
	char tmp_param[32];
	for(i = 0; i< nr_blk; i++){
		if(blk_buf[i].unuse == 1) continue;
		debug("constprop~test0, i = %d\n", i);

		for(j = blk_buf[i].start ;j < blk_buf[i].end ; j++ ){

			if(array.item[j]->type == IR_ASSIGN){  

				debug("add2def(var_line blk): %d %d\n", j, i);

			}else if(strPartOf(array.item[j]->pTo,"|+|-|*|/|>|>=|==|!=|<=|<|") ){

				p2_find = 0;
				pToo_find = 0;

				int tmp_i_lead;
				tmp_i_lead = blk_buf[i].lead;

				for(k = j-1; k >= blk_buf[tmp_i_lead].start; k-- ){
					debug("array.item[%d]->type = %d op = %s\n",k,array.item[k]->type, array.item[k]->op);
					if((array.item[k]->type == IR_ASSIGN) || (array.item[k]->type == IR_DEC ) ){
						if(!p2_find &&  !strcmp(array.item[j]->p2, array.item[k]->op)){

							if(strMember(array.item[k]->p2[0],DIGIT)){
								strncpy(array.item[j]->p2, array.item[k]->p2, 32 );
							}else{
								array.item[k]->use = 1;	
							}
							p2_find = 1;

						}


						if( (array.item[k]->type == IR_DEC)&& !p2_find && !strcmp(array.item[j]->p2, array.item[k]->p1) ){

							if(strMember(array.item[k]->p2[0],DIGIT)){
								strncpy(array.item[j]->p2, array.item[k]->p2, 32 );
							}else{
								array.item[k]->use = 1;	
							}
							p2_find = 1;
						}

						if(!pToo_find && !strcmp(array.item[j]->pToo, array.item[k]->op)){

							if(strMember(array.item[k]->p2[0],DIGIT)){
								strncpy(array.item[j]->pToo, array.item[k]->p2, 32 );
							}else{
								array.item[k]->use = 1;			
							}
							pToo_find = 1;

						}
					}
				}	

			}else if(array.item[j]->type == IR_JMP){
				debug("array.item[%d]->p1[0] = %c\n",j,array.item[j]->p1[0]);
				if(array.item[j]->p1[0] == '!'){
					sprintf(tmp_param, "%s", array.item[j]->p1+1 );
					debug("tmp_param = %s\n", tmp_param);

					if(array.item[j-1]->type == IR_ASSIGN){

						if(!strcmp(tmp_param ,array.item[j-1]->op)){

							if(strMember(array.item[j-1]->p2[0],DIGIT) ){


								sprintf(array.item[j]->p1,"!%s", array.item[j-1]->p2 );
								if(array.item[j]->p1[1] == '0'){
									strncpy(array.item[j]->p1, "1", 32 );
								}else{
									strncpy(array.item[j]->p1, "0", 32 );
								}
							}
						}
					}

				}else{
					if(array.item[j-1]->type = IR_ASSIGN && !strcmp(array.item[j]->p1 ,array.item[j-1]->op) && strMember(array.item[j-1]->p2[0],DIGIT) ){

						strncpy(array.item[j]->p1, array.item[j-1]->p2, 32 );
						if(array.item[j]->p1[0] == '0'){
							strncpy(array.item[j]->p1, "0", 32 );
						}else{
							strncpy(array.item[j]->p1, "1", 32 );
						}
					}
					if(array.item[j]->p1[0] == '0'){
					}

				}

			}
		}

	}	
}


void copyprop(){

	int i = 0;
	int j = 0;
	int k = 0;
	int p2_find = 0;
	int pToo_find = 0;

	for(i = 0; i< nr_blk; i++){
		if(blk_buf[i].unuse == 1) continue;

		for(j = blk_buf[i].start ;j < blk_buf[i].end ; j++ ){

			if(array.item[j]->type == IR_ASSIGN){

				p2_find = 0;

				int tmp_i_lead;
				tmp_i_lead = blk_buf[i].lead;
				for(k = j-1; k >= blk_buf[tmp_i_lead].start; k-- ){
					if( (array.item[k]->type == IR_ASSIGN) || (array.item[k]->type == IR_DEC) ){

						if(!p2_find &&  !strcmp(array.item[j]->p2, array.item[k]->op)){

							strncpy(array.item[j]->p2, array.item[k]->p2, 32 );
							p2_find = 1;
							break;


						}

						if( (array.item[k]->type == IR_DEC) && !p2_find && !strcmp(array.item[j]->p2, array.item[k]->p1) ){

							strncpy(array.item[j]->p2, array.item[k]->p1, 32 );
							p2_find = 1;
							break;
						}
					}
				}	
			}
		}
	}	
}

int use_def_check(int i, int j){

	if( !strcmp(array.item[i]->op,  array.item[j]->p2)	){
		return 1;
	}
	if( !strcmp(array.item[i]->op, array.item[j]->pToo) ){
		return 1;
	}
}

void dce_local(){

	int i = 0;
	int j = 0;
	int j_used = 0;

	int lead_start = blk_buf[ blk_buf[nr_blk-1].lead ].start;
	for(i = nr_instr-1 ; i>lead_start; i--){

		if(array.item[i]->type >= IR_ASSIGN && array.item[i]->type <= IR_DIV){
			for(j=i;j>lead_start;j--){
				j_used=use_def_check(i,j);
				if(j_used) array.item[j]->use=1;
			}	
		}
	}
}


void block_analysis(){

	int i = 0;
	int j = 0;
	char tmp_goto_param[32];
	int tmp_p1_num;
	int tmp_pTo_num;
	char tmp_pTo_str[32];
	int inst_param;
	for(i=0;i<nr_blk;i++){
		blk_buf[i].if_jump_in = 0;
		blk_buf[i].use = 0;
	}

	for(i = 0; i<nr_blk; i++ ){
		debug("i = %d, nr_blk = %d\n",i,nr_blk);

		inst_param = blk_buf[i].end - 1;
		debug("inst_param : %d\n",inst_param);

		if(array.item[inst_param]->type == IR_GOTO){
			debug("IR_GOTO\n");
			sprintf(tmp_pTo_str, "%s", array.item[j]->p1+2 ); 
			tmp_pTo_num = atoi(tmp_pTo_str);
			debug("tmp_pTo_num : %d\n",tmp_pTo_num);
			for(j=0;j<nr_blk;j++){
				debug("blk_buf[%d].goto_param = %d\n",j,blk_buf[j].goto_param);
				if(blk_buf[j].goto_param == tmp_pTo_num){
					debug("blk_buf[%d].goto_param == tmp_pTo_num\n",j);
					blk_buf[j].if_jump_in = 1;
					blk_buf[j].use = 1;
					blk_buf[i].if_jump_out = 1;
				}
			}
		}else if(array.item[inst_param]->type == IR_JMP){
			debug("IR_JMP\n");

			if(strMember(array.item[inst_param]->p1[0],DIGIT) ){
				tmp_p1_num = atoi(array.item[inst_param]->p1);
				debug("array.item[%d] tmp_p1_num : %d\n",inst_param,tmp_p1_num);

				if( tmp_p1_num ){
					blk_buf[i+1].unuse = 1;
					debug("blk_buf[%d].unuse = 1\n",i+1);
					sprintf(tmp_pTo_str, "%s", array.item[inst_param]->pTo+2 );
					tmp_pTo_num = atoi(tmp_pTo_str);

					for(j=0;j<nr_blk;j++){
						debug("blk_buf[%d].goto_param = %d\n",j,blk_buf[j].goto_param);
						if(blk_buf[j].goto_param == tmp_pTo_num){
							debug("blk_buf[%d].goto_param == tmp_pTo_num\n",j);
							blk_buf[j].if_jump_in = 1;
							blk_buf[j].use = 1;
							blk_buf[i].if_jump_out = 1;
						}
					}
				}else{
					blk_buf[i+1].use = 1;

					sprintf(tmp_pTo_str, "%s", array.item[inst_param]->pTo+2 );
					tmp_pTo_num = atoi(tmp_pTo_str);
					debug("tmp_pTo_num : %d\n",tmp_pTo_num);

					for(j=0;j<nr_blk;j++){
						debug("blk_buf[%d].goto_param = %d\n",j,blk_buf[j].goto_param);
						if(blk_buf[j].goto_param == tmp_pTo_num){
							debug("blk_buf[%d].goto_param == tmp_pTo_num\n",j);
							blk_buf[j].unuse = 1;
						}
					}
				}	
			}
		}
	}
	int tmp_lead = 0;

	for(i = 1; i<nr_blk; i++ ){

		if(blk_buf[i-1].if_jump_out != 0){
			if(blk_buf[i].use == 0){

			}
		}
		if(blk_buf[i-1].unuse == 1){
			if(blk_buf[i].use == 0){

			}
		}
		if(blk_buf[i].if_jump_in == 1){
			tmp_lead = i;
		}
		blk_buf[i].lead = tmp_lead;	
		debug("blk_buf[%d].lead = %d\n",i, blk_buf[i].lead);
	}
}


void optimize(){

	int i = 0;
	for(i=0; i<3; i++){
		constfold();
		constprop();
		copyprop();
	}
	block_analysis();
}


void optimize_in_block()
{
	debug("nr_instr : %d\n", nr_instr);
	debug("array.item[0]->op = %s\n",array.item[0]->op );
	nr_blk = block_partition(blk_buf, array.item, nr_instr);
	int i = 0;
	int blk = 0;
	int blk_inst_size = 0;
	for(blk = 0; blk < nr_blk; blk++ ){
		blk_inst_size = blk_buf[blk].end - blk_buf[blk].start;
		debug("blk_buf[%d].start : %d, end : %d, blk_inst_size : %d\n", blk, blk_buf[blk].start, blk_buf[blk].end, blk_inst_size);
		if(blk_inst_size != 0){
			blk_buf[blk].def =  malloc( blk_inst_size*sizeof(int*) );
		}
		for(i = 0; i < blk_inst_size; i++){
			blk_buf[blk].def[i] = malloc ( sizeof (int) );
		}
	}

	debug("nr_blk : %d\n", nr_blk);
	block_analysis();

	optimize();

	for(blk = nr_blk-1; blk >= 0; blk--){
		blk_inst_size = blk_buf[blk].end - blk_buf[blk].start;
		for(i = blk_inst_size-1; i >= 0; i--){
			debug("free blk_buf[%d].defnum = %d, i = %d \n", blk, blk_buf[blk].defnum, i );

			free(blk_buf[blk].def[i]);
		}
		debug("free blk_buf[%d].def\n", blk );

		free(blk_buf[blk].def);
	}
}


void gen_or(char *asmFileName){
	int i = 0;

	for(i = 0; i< nr_blk; i++   ){
		char str[12];
		sprintf(str, "%d", i);
		char goto_param_str[32];
		strncpy(goto_param_str ,m_itoa(blk_buf[i].goto_param), 32);
		char lead_param_str[32];

		strncpy(lead_param_str ,m_itoa(blk_buf[i].lead), 32);
		debug("blk_buf[%d].goto_param = %d\n",i,blk_buf[i].goto_param);
		debug("goto_param_str : %s\n",goto_param_str);

#ifdef DEBUG
		rewrite(asmFileName, "// --- block - goto - lead ", str, goto_param_str , lead_param_str ," ----------");
#endif

		debug("blk_buf[%d].unuse = %d\n",i,blk_buf[i].unuse);
		if(blk_buf[i].unuse == 1 ){
			continue;
		}else{
			int j = 0;
			debug("blk_buf[%d].start = %d\n", i,blk_buf[i].start);
			debug("blk_buf[%d].end = %d\n", i,blk_buf[i].end);
			for(j = blk_buf[i].start ; j<blk_buf[i].end ; j++){
				debug("array.item[%d]->unuse = %d, op = %s \n",j,array.item[j]->unuse, array.item[j]->op);
				if( array.item[j]->unuse == 1 ){
					continue;
				}else{
					rewrite(asmFileName,
							array.item[j]->op,
							array.item[j]->p1,
							array.item[j]->p2,
							array.item[j]->pTo,
							array.item[j]->pToo);
				}
			}
		}
	}

}



void pcodeToAsm_2(char *asmFileName, char* op, char* p1, char* p2, char* pTo, char* pToo){

	IR * ptr_Quad = malloc(sizeof( IR));

	if (array.count == array.size){
		int newSize = array.size*2;
		IR **newItems = malloc(newSize*sizeof(IR));
		memcpy(newItems, array.item, array.size*sizeof(IR)); 
		free(array.item);
		array.item = newItems;
		array.size = newSize;
		debug("array.size : %d\n",array.size);
	}

	strncpy(ptr_Quad->op, op, 32 );
	strncpy(ptr_Quad->p1, p1, 32 );
	strncpy(ptr_Quad->p2, p2, 32 );
	strncpy(ptr_Quad->pTo, pTo, 32 );
	strncpy(ptr_Quad->pToo, pToo, 32 );

	ptr_Quad->unuse = 0;

	if( strPartOf(p1, "||") ){
		ptr_Quad->type = IR_LABEL;
		debug("IR_LABEL ");
	}else if( strEqual(op, "op_if(") ){
		ptr_Quad->type = IR_JMP;
		debug("IR_JMP ");
	}else if( strEqual(op, "op_goto") ){
		ptr_Quad->type = IR_GOTO;
		debug("IR_GOTO ");
	}else if( strEqual(op, "op_dec") ){
		ptr_Quad->type = IR_DEC;
		debug("IR_DEC ");
	}else if( strEqual(p1, "=") && strPartOf(pTo, "||" )){
		ptr_Quad->type = IR_ASSIGN;
		debug("IR_ASSIGN ");
	}else if( strEqual(pTo, "+") ){
		ptr_Quad->type = IR_ADD;
		debug("IR_ADD ");
	}else if( strEqual(pTo, "-") ){
		ptr_Quad->type = IR_SUB;
		debug("IR_SUB ");
	}else if( strEqual(pTo, "*") ){
		ptr_Quad->type = IR_MUL;
		debug("IR_MUL ");
	}else if( strEqual(pTo, "/") ){
		ptr_Quad->type = IR_DIV;
		debug("IR_DIV ");
	}else if( strEqual(pTo, ">") ){
		ptr_Quad->type = IR_BGT;
		debug("IR_BGT ");
	}else if( strEqual(pTo, ">=") ){
		ptr_Quad->type = IR_BGE;
		debug("IR_BGE ");
	}else if( strEqual(pTo, "==") ){
		ptr_Quad->type = IR_BEQ;
		debug("IR_BEQ ");
	}else if( strEqual(pTo, "!=") ){
		ptr_Quad->type = IR_BNE;
		debug("IR_BNE ");
	}else if( strEqual(pTo, "<=") ){
		ptr_Quad->type = IR_BLE;
		debug("IR_BLE ");
	}else if( strEqual(pTo, "<") ){
		ptr_Quad->type = IR_BLT;
		debug("IR_BLT ");
	}else{
		ptr_Quad->type = IR_NULL;
		debug("IR_NULL ");
	}	

	debug("ptr_Quad->type : %d \n",ptr_Quad->type);
	debug("nr_instr : %d\n",nr_instr);	

	array.item[nr_instr++]=ptr_Quad;
}


int spliteLineStr(char* bufIn, char* op, char* p1, char* p2, char* pTo, char* pToo){
	char szBuf[6][32];
	int n = 0;
	int idx = 0;
	int i = 0, j = 0, k = 0;
	int nLen = strlen(bufIn); 

	for(i = 0; i<6; i++){
		szBuf[i][0] = '\0';
	}
	for(i = 0; i < nLen; i++){
		if (n >= 6){ printf("Error\n"); }

		if(bufIn[i] == ' '){
			j = 0;
		}else{
			j = 1;
		}

		if(k == 0){
			if ( j == 1 ){ k = 1;}

		}else if(k == 1){
			if (j == 0){
				bufIn[i] = '\0';
				strncpy(szBuf[n++], bufIn + idx, 32);
				idx = i + 1;

				k = 2;

			}
			if(n <= 1){
				debug("szBuf[0] = %s\n",szBuf[0]);
				if(!strcmp(szBuf[0],"//")){
					debug("szBuf[0] = %s\n",szBuf[0]);				
					return -1;
				}
			}
		}else if(k == 2){
			if (j == 1){k = 1; idx = i;}
		}

	}
	bufIn[i-1] = '\0';
	strncpy(szBuf[n++], bufIn+idx, 32);
	strncpy(op, szBuf[0], 32 );
	strncpy(p1, szBuf[1], 32);
	strncpy(p2, szBuf[2], 32);
	strncpy(pTo, szBuf[3], 32);
	strncpy(pToo, szBuf[4], 32);
	return 0;
}


void fileToLine(char *fileName, char* asmFileName){

	FILE *fp;

	char op[32];
	char p1[32];
	char p2[32];
	char pTo[32];
	char pToo[32];

	char StrLine[1024];

	static int lineNum = 1;
	static int size = 50;
	int chk_sLS = 0;

	if((fp = fopen(fileName,"r")) == NULL){
		printf("file open error!");
		return;
	}

	fgets(StrLine, size, fp);
	while( !feof(fp) ){

		chk_sLS = spliteLineStr(StrLine, op, p1, p2, pTo, pToo);
		if (chk_sLS) continue;
		pcodeToAsm_2(asmFileName, op, p1, p2, pTo, pToo);	
		fgets(StrLine, size, fp);
	}

	fclose(fp);

	optimize_in_block();
	int i = 0;
	for(i = 0; i<nr_instr;  i++ ){
		debug("%s\n",array.item[i]->op);
	}
	gen_or(asmFileName);
}


void arrayNew(){
	array.count = 0;

	array.size = 22240;

	array.item = malloc(array.size*sizeof(IR));
}


void compile(char *cFile, char *asmFileName){

	debug("compile file:%s \n", cFile);

	arrayNew();

	fileToLine(cFile, asmFileName);
}


int main(int argc, char *argv[]   ){
	char *cFile;
	char *asmFile;
	if(argc == 3){
		cFile = argv[1];
		asmFile = argv[2];
	}else{
		printf("usage : ./luop test.ir test.or\n");
		return 0;

	}
	compile(cFile, asmFile);
	return 0;
}



