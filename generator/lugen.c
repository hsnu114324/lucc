#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define strMember(ch, set) (strchr(set, ch) != NULL)

#define strEqual(str1, str2) (strcmp(str1, str2)==0)

#define DIGIT "0123456789"    


unsigned char strHead(char *str, char *head) {
        return (strncmp(str, head, strlen(head))==0);
}

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

struct linklist {
	char num[20];
	int  score;
	struct linklist *llink;
	struct linklist *rlink;
}linklistTable[30];

struct linklist *ptr, *head, *tail, *current_n, *prev;

int reg_cot = 0;
char reg_cot_str[20];
int dec_cot = 0;
char dec_cot_str[20];
char dec_str[10][20];
char dec_title_str[10][20];
char arg_str[10][20];
char reg_str[10][20];
char mov_str[20] = "\tmov $";
char sub_str[20] = "\tsub $";
long long arg_mask = 0;
long long reg_mask = 0;
int arg_num = 0;
int dec_num = 0;
int dec_state = 0;
int arg_state = 0;
int i = 0;
char *REGS[] = {"%rdi","%rsi","%rdx","%rcx","%r8","%r9"};
char if_str[20];
char dec_temp_str[20];
char if_stmt[4][20];
char Arithmetic[10][20];
int Arithmetic_num = -1;

char Ari_symbol[30];
int Ari_sym_num = 0;


void linklist_add_star(int Arithmetic_num ){

	debug("linklist_add_star\n");
	debug("Arithmetic_num : %d\n" ,Arithmetic_num);
	current_n = &linklistTable[Arithmetic_num];

	ptr = &linklistTable[Arithmetic_num];
	while( ptr->rlink != NULL ){
		ptr = ptr->rlink;
		debug("-> socre : %d ",ptr->score );
	}
	debug("\n");

	ptr = &linklistTable[Arithmetic_num];
	while( ptr->rlink != NULL ){
		ptr = ptr->rlink;
		debug("-> num : %s ",ptr->num );
	}
	debug("\n");


	ptr = (struct linklist *) malloc( sizeof(struct linklist) );
	sprintf(ptr->num, "@" );
	ptr->score = 0;

	head = current_n;
	prev = head->rlink;
	current_n = current_n->rlink->rlink;

	prev->rlink = ptr;
	ptr->llink = prev;
	ptr->rlink = current_n;
	current_n->llink = ptr;

	while( (current_n != head)&&(current_n->rlink != NULL  )  ){

		current_n = current_n->rlink;
	}

	ptr = (struct linklist *) malloc( sizeof(struct linklist) );
	sprintf(ptr->num, "#" );

	current_n->rlink = ptr;
	ptr->llink = current_n ;
	ptr->rlink = NULL;
	ptr->score = 0;
}


void linklist_add(int Arithmetic_num, char*  Arithmetic, int type){

	char Arithmetic_token[20];
	int Arithmetic_token_num;
	if( Arithmetic[0] == '.' ){
		debug("op_dot\n");
		sprintf(Arithmetic_token,"%s",Arithmetic+2);
		debug("Arithmetic_token : %s \n",Arithmetic_token);
		Arithmetic_token_num = atoi(Arithmetic_token);
		debug("Arithmetic_token_num : %d \n",Arithmetic_token_num);
		ptr = &linklistTable[Arithmetic_token_num]; 
		ptr = ptr->rlink;

		debug("Arithmetic_num : %d\n:", Arithmetic_num);
		current_n = &linklistTable[Arithmetic_num];

		while( current_n->rlink != NULL ){
			current_n = current_n->rlink;
			debug("-> socre : %d ",current_n->score );
		}

		current_n->rlink = ptr;

		debug("op_dot done\n");
		return;

	}else{
		ptr = (struct linklist *) malloc( sizeof(struct linklist) );

		debug("Arithmetic[0] : %c \n", Arithmetic[0]);	
		if(strMember(Arithmetic[0], DIGIT)){
			debug("digit\n");
			sprintf(Arithmetic_token,"%s",Arithmetic);
			Arithmetic_token_num = atoi(Arithmetic_token);

			ptr->num[0] = '\0';

		}else{
			debug("not digit\n");

			if( strMember(Arithmetic[0], "|+|-|*|/|%|&|||^|" )  ){

				Arithmetic_token_num = 0;
				strcpy( ptr->num, Arithmetic );

			}else{

				int i = 0;
				int dec_title_num = -1;
				debug("dec_num : %d\n",dec_num);
				for(i=0;i<dec_num;i++){

					debug("dec_title_str[%d] : %s\n", i, dec_title_str[i] );
					debug("Arithmetic : %s\n", Arithmetic );
					if(strEqual(Arithmetic, dec_title_str[i])){
						debug("Arithmetic find! \n");
						dec_title_num = i;
						dec_title_num += 1;
						debug("dec_title_num : %d \n", dec_title_num);			
					}
				}

				if(dec_title_num == -1){dec_title_num = 0;}	
				Arithmetic_token_num = 8*dec_title_num;
				strcpy( ptr->num, Arithmetic );
			}
		}
	}	
	ptr->score = Arithmetic_token_num;
	current_n = &linklistTable[Arithmetic_num];
	head = current_n;

	if( current_n->rlink == NULL ){
		current_n->rlink = ptr;
		ptr->llink = current_n;
	}else{
		current_n = head->rlink;
	}

	while( (current_n != head)&&(current_n->rlink != NULL  )  ){
		prev = current_n;
		current_n = current_n->rlink;
	}

	ptr->llink = current_n;
	ptr->rlink = NULL;
	current_n->rlink = ptr;
}


void splitLineStr(char* bufIn, char* op, char* p1, char* p2, char* pTo, char* pToo ){

	char szBuf[6][32];
	int n   = 0;
	int idx = 0 ;
	int i = 0, j = 0, k = 0;
	int nLen = strlen (bufIn) ;
	int global_string_continue = 0;

	for(i = 0; i < 6; i++){
		szBuf[i][0] = '\0';
	}

	for (i = 0 ; i < nLen ; i++)
	{

		if (n >= 6){ debug("Error\n");  }

		if(bufIn[i] == '"' ){
			if(global_string_continue){
				global_string_continue = 0;
				k = 1;
			}else{
				global_string_continue = 1;
				continue;
			}
		}else if(global_string_continue){
			continue;
		}

		if(bufIn[i] == ' '){
			j = 0;
		}else{
			j = 1;
		}

		if(k == 0 ){

			if (j == 1 ){ k = 1;  }		
		}else if(k == 1){

			if (j == 0){

				bufIn[i] = '\0' ;
				strncpy (szBuf[n++], bufIn + idx, 32) ;
				idx = i + 1 ;
				k = 2;
			} 
		}else if(k == 2){

			if (j == 1){k = 1; idx = i;   } 
		}
	}

	if(global_string_continue){
		debug("global_string_continue Error\n");
	}

	bufIn[i-1] = '\0' ; 

	strncpy (szBuf[n++], bufIn + idx, 32) ;

	strncpy(op, szBuf[0], 32  );
	strncpy(p1, szBuf[1], 32  );
	strncpy(p2, szBuf[2], 32  );
	strncpy(pTo, szBuf[3], 32  );
	strncpy(pToo, szBuf[4], 32  );
}


void rewrite(char *asmFileName, char* op, char* p1, char* p2, char* pTo){

	FILE *asmFile_fp;

	if((asmFile_fp = fopen(asmFileName,"at+")) == NULL) 
	{ 
		printf("file open error!"); 
		return; 
	} 

	char *realOp = op;
	if (strEqual(op, "LD" ))
		realOp = "LDI";

	fprintf(asmFile_fp, "%s %s %s %s\n", realOp, p1, p2, pTo);

	fclose(asmFile_fp);
}

int stackpos = 0;

void push(char*asmFileName, char *reg, char *comment){

        rewrite(asmFileName, "\tpush", reg, "", comment);
        stackpos +=8;
}


void pointer_arith(char* asmFileName, int num, int size){

	char num_str[20];	
	char size_str[20];
	sprintf(num_str, "$%d,", num);
	sprintf(size_str, "$%d,", size);

	rewrite(asmFileName, "\tmov", num_str, "%eax","");
	rewrite(asmFileName, "\timul", size_str, "%rax","");

	rewrite(asmFileName, "\tmov", "%rax,", "%rcx","");
	rewrite(asmFileName, "\tpop", "%rax", "","");
	rewrite(asmFileName, "\tadd", "%rcx,", "%rax","");
}


void int_arith(char* asmFileName, int Arithmetic_num   ){

	int variable_twice = 0;
	char temp_arg_str[20];
	temp_arg_str[0] = '\0';

	debug("Arithmetic_num :  %d \n", Arithmetic_num);
	ptr = &linklistTable[Arithmetic_num];

	debug("ptr->num : %s\n", ptr->num);
	debug("ptr->num[0] : %c\n", ptr->num[0]  );

	while(ptr->rlink != NULL){
		ptr = ptr->rlink;
		if( ptr->num[0] == '\0' ){

			debug("ptr->num == NULL~\n");
			debug("ptr->score : %d\n", ptr->score);

			sprintf(temp_arg_str, "$%d,", ptr->score);
			debug("temp_arg_str : %s\n", temp_arg_str);

			rewrite(asmFileName, "\tmov", temp_arg_str, "%eax","" );
			if( ptr->rlink->num[0] == '\0'  ){
				rewrite(asmFileName, "\t", "push", "%rax", "\t# int_arith_460" );
			}else{
				debug("ptr->rlink->num[0] : %c\n", ptr->rlink->num[0]);
			}
		}else {

			if( strMember( ptr->num[0], "|+|-|*|/|%|^|"  ) ){
				char temp_op[20];

				debug("ptr->num != NULL\n");
				rewrite(asmFileName, "\tmov", "%rax,", "%rcx","" );
				rewrite(asmFileName, "\tpop", "%rax", "","" );

				debug("ptr->num : %s\n", ptr->num);
				debug("ptr->num[0] : %c\n", ptr->num[0]  );

				if( ptr->num[0] == '+' ){
					strcpy(temp_op, "\tadd");	
					rewrite(asmFileName, temp_op, "%rcx,", "%rax","" );
				}else if( ptr->num[0] == '-' ){
					strcpy(temp_op, "\tsub");	
					rewrite(asmFileName, temp_op, "%rcx,", "%rax","" );
				}else if( ptr->num[0] == '*' ){
					strcpy(temp_op, "\timul");	
					rewrite(asmFileName,  temp_op, "%rcx,", "%rax","" );
				}else if( ptr->num[0] == '/' ){
					strcpy(temp_op, "\tidiv");	
					rewrite(asmFileName, "\tmov", "$0,", "%edx","" );
					rewrite(asmFileName, "\tidiv", "%rcx,","","");

				}else if( ptr->num[0] == '%' ){
					strcpy(temp_op, "\tidiv");	
					rewrite(asmFileName, "\tmov", "$0,", "%edx" ,"");
					rewrite(asmFileName, "\tidiv", "%rcx,","","");
					rewrite(asmFileName, "\tmov", "%edx", "%eax","" );
				}else if( ptr->num[0] == '^' ){
					strcpy(temp_op, "\rxor");	
					rewrite(asmFileName,  temp_op, "%rcx,", "%rax","" );
				}else{
					debug("Error..\n");	
					return;
				}

			}else if( strMember( ptr->num[0], "|@|" )  ){

				debug("|@|ptr->num : %s\n", ptr->num);
				debug("|@|ptr->num[0] : %c\n", ptr->num[0]  );

				int ptr_num = 0;
				int ptr_size = 0;

				do{
					prev = ptr;
					ptr = ptr->rlink;
					ptr_num = ptr->score;
					debug("ptr_num : %d\n",ptr_num);
					ptr = ptr->rlink;
					ptr_size = ptr->score;
					debug("ptr_size : %d\n",ptr_size);

					if( ptr->rlink->num[0] == '*' ){
						if(  ptr->rlink->rlink->num[0] == '+'  ){
							ptr = ptr->rlink->rlink->rlink;	
							pointer_arith(asmFileName, ptr_num, ptr_size );
						}else{
							debug("ptr->rlink->rlink->num[0] = %c \n", ptr->rlink->rlink->num[0]);
						}
					}else{
						debug("ptr->rlink->num[0] = %c \n", ptr->rlink->num[0]);
					}

					debug("ptr_size End\n");
					debug("|#|ptr->num[0] : %c\n",ptr->num[0]);
					if( !strMember(ptr->num[0], "|#|") ){  
						rewrite(asmFileName, "\tpush ", "%rax", " ", "\t# int_arith_550" );
					}
				}while( !strMember(ptr->num[0], "|#|") );

				rewrite(asmFileName, "\tmov ", "(%rax),", "%ecx", "\t# int_arith_551" );
				rewrite(asmFileName, "\tmov ", "%rcx,", "%rax", "\t# int_arith_552" );

				debug("End_ptr_@\n");

			}else if(ptr->score == 0){
				sprintf(temp_arg_str, "%s(%%rip),", ptr->num);
				debug("temp_arg_str : %s", temp_arg_str);

				rewrite(asmFileName, "\tlea ", temp_arg_str, "%rax", "\t# int_arith_557" );
				rewrite(asmFileName, "\tpush %rax", "", "\t# int_arith_558","" );
			}else{

				sprintf(temp_arg_str, "-%d(%%rbp),", ptr->score);
				debug("temp_arg_str : %s", temp_arg_str);

				rewrite(asmFileName, "\tmov", temp_arg_str, "%rax", "\t# int_arith_563" );
				if(variable_twice == 0){
					rewrite(asmFileName, "\tpush %rax", "", "\t# int_arith_564","" );
					variable_twice = 1;
				}else{
					variable_twice = 0;
				}
			}
		}

		if(ptr->rlink != NULL){
			debug("ptr->rlink->score : %d \n", ptr->rlink->score);
			debug("ptr->rlink->num : %s \n", ptr->rlink->num);
		}else{
			debug("ptr->rlink == NULL \n");
		}
	}	
}


void pcodeToAsm(char *asmFileName, char* op, char* p1, char* p2, char* pTo, char* pToo  ){

	if(dec_state == 1 && !strEqual(op, "op_dec")  ){
		debug("not op_dec!\n");
		dec_state = 0;

		int dec_num_area = 0;
		dec_num_area = (1+dec_num)*dec_num/2;	
		int dec_sub_num = 8 * dec_num_area;
		char dec_sub_num_str[20];
		sprintf(dec_sub_num_str, "%d", dec_sub_num);
		strcat(sub_str,dec_sub_num_str);
		int len = strlen(sub_str);
		debug("strlen(mov_str) : %d\n",len);
		sub_str[len] = ','; 
		rewrite(asmFileName, sub_str , "%rsp" , "\t# not op_dec_787", "" );

		for(i = 0; i<dec_num; i++ ){

			rewrite(asmFileName, dec_str[i], "%eax","\t# not op_dec_794", "");
			dec_cot -= 8;
			sprintf(dec_cot_str, "%d", dec_cot);
			strcat(dec_cot_str, "(%rbp) ");
			rewrite(asmFileName, "\tmov", "%eax,", dec_cot_str, "\t# mot op_dec_800");
		}
	}

	debug("op = %s\n", op);

	if(arg_state == 1 && !strEqual(op, "op_arg") && !strEqual(op, "op_call") ){
		debug("Error IR should be  arg..arg,call\n");
		return;
	}

	if(strHead(op, "*" ) ){
		int temp_num = 0;
		char temp_num_str[20];
		debug("op_star\n");

		char temp_arg_str[20];
		temp_arg_str[0] = '\0';

		sprintf(Arithmetic[0], "%s", op+3);
		debug("Arithmetic[0] : %s\n", Arithmetic[0]);
		Arithmetic_num = atoi(Arithmetic[0]);
		linklist_add_star(Arithmetic_num);

		sprintf(temp_arg_str, "$%s,", p2);
		debug("temp_arg_str : %s", temp_arg_str);

		rewrite(asmFileName, "\tmov", temp_arg_str, "%eax", "\t# op_star_845" );
		rewrite(asmFileName, "\t", "push", "%rax" , "\t# op_star_846" );

		sprintf(temp_num_str, "%s", op+3);
		debug("temp_num_str : %s\n", temp_num_str);
		temp_num = atoi(temp_num_str);	
		debug("temp_num : %d\n", temp_num);
		int_arith(asmFileName, temp_num);

		rewrite(asmFileName, "\tmov", "(%rsp),", "%rcx", "\t# op_star_854" );
		rewrite(asmFileName, "\tmov", "%ecx,", "(%rax)", "\t# op_star_855" );
		rewrite(asmFileName, "\tpop", "%rax" , "" ,"\t# op_star_856" );

	}else if(strHead(op, ".") ){
		int len = strlen(op);

		char dot_check[20];
		dot_check[0] = '\0';
		strncpy(dot_check, op+len-1, 2);

		debug("p1 : %s\n", p1);
		debug("if_stmt[0] : %s\n", if_stmt[0]);
		if(strHead(dot_check, ":")  ){
			debug("op -> token label\n");
			rewrite(asmFileName, op, "", "", "" );

		}else if(strEqual(op, if_stmt[0]) ){
			debug("pTo : %s \n", pTo);
			if( strPartOf(pTo, "||")  ){
				debug("pTo == NULL \n");


				if( p2[0] == '*' ){

					sprintf(Arithmetic[0], "%s", op+2);
					debug("Arithmetic[0] : %s\n", Arithmetic[0]);
					Arithmetic_num = atoi(Arithmetic[0]);
					strcpy(Arithmetic[1], p2+1);

					debug("Arithmetic[1]: %s\n",Arithmetic[1]);
					linklist_add(Arithmetic_num, Arithmetic[1], 1);
					linklist_add_star(Arithmetic_num);
				}else{
					debug("Error~\n");
					return;
				}

			}else if(strPartOf(pTo, "|==|!=|<=|>=|<|>|")  ){
				debug("op if_stmt equal!\n");
				strcpy(if_stmt[1], p2);
				strcpy(if_stmt[2], pTo);
				strcpy(if_stmt[3], pToo);

			}else if(strPartOf(pTo, "|+|-|*|/|%|&|||^|" ) ){

				debug("op_3 equal! \n ");

				strcpy(Arithmetic[1], p2);
				strcpy(Arithmetic[2], pTo);
				strcpy(Arithmetic[3], pToo);

				if( Arithmetic[2][1] != '\0' ){
					debug("Arithmetic[2][1] != \\0 Error\n");
				}
				debug("Ari_sym_num : %d\n",Ari_sym_num);
				strncat( Ari_symbol  ,Arithmetic[2], 1);

				Ari_sym_num += 1;

				sprintf(Arithmetic[0], "%s", op+2);
				debug("Arithmetic[0] : %s\n", Arithmetic[0]);
				Arithmetic_num = atoi(Arithmetic[0]);
				debug("Arithmetic_num : %d\n", Arithmetic_num);

				debug("Arithmetic[1]: %s\n",Arithmetic[1]);

				linklist_add(Arithmetic_num, Arithmetic[1], 1);
				debug("Arithmetic[3]: %s\n",Arithmetic[3]);
				linklist_add(Arithmetic_num, Arithmetic[3], 3);
				debug("Arithmetic[2]: %s\n",Arithmetic[2]);
				linklist_add(Arithmetic_num, Arithmetic[2], 2);

				debug("Arithmetic_num : %d\n", Arithmetic_num);

				ptr = &linklistTable[Arithmetic_num];
				while( ptr->rlink != NULL ){
					ptr = ptr->rlink;
					debug("-> socre : %d ",ptr->score );
				}
				debug("\n");

				ptr = &linklistTable[Arithmetic_num];
				while( ptr->rlink != NULL ){
					ptr = ptr->rlink;
					debug("-> num : %s ",ptr->num );
				}
				debug("\n");

			}else{

				debug("...Error\n");
				return; 
			}
		}else{ 

			debug("pTo: %s\n",pTo);
			if( strPartOf(pTo, "||")  ){
				debug("pTo == NULL \n");

				if( p2[0] == '"' ){

					char temp_op[20];
					sprintf(temp_op, "%s:", op);

					rewrite(asmFileName, "\t.data", "", "","");
					rewrite(asmFileName, temp_op, "", "","");
					rewrite(asmFileName, "\t.string", p2, "","");
					rewrite(asmFileName, "\t.text", "", "","");
					rewrite(asmFileName, ".global main", "", "","");
					rewrite(asmFileName, "main:", "", "","");
					push(asmFileName, "%rbp", "\t# int_arith_2122");
					rewrite(asmFileName, "\tmov","%rsp,", "%rbp", "");
					return ;
				}else{
					debug("..Error\n");
					return;	
				}
			}
			debug(".Error\n");
			return; 
		}
	}else if(strEqual(op, "op_goto")){
		rewrite(asmFileName, "\tjmp", p1, "","");
	}else if(strEqual(op, "op_if(")){
		int je_use = 1;
		if(strHead(p1,"!")){
			je_use = 1;
			char tmp_p1[20];
			sprintf(tmp_p1, "%s", p1);
			sprintf(p1, "%s", tmp_p1+1);
		}
		if(strHead(p1, ".") ){

			if(!strEqual(p1, if_stmt[0])){
				debug("op_if if_stmt error!\n");
				return;
			}

			int j = -1;
			int k = -1;
			for(i=0;i<dec_num;i++){

				debug("dec_title_str[%d] : %s\n", i, dec_title_str[i] );
				if(strEqual(if_stmt[1], dec_title_str[i])){
					debug("if_smtm[1] find! \n");
					j = i;
				}
				if(strEqual(if_stmt[3], dec_title_str[i])){
					debug("if_smtm[3] find! \n");
					k = i;
				}
			}

			debug("j : %d\n",j); 
			debug("k : %d\n",j); 
			char if_cot_str[20];
			if(j != -1){

				debug("if_stmt[1] confirm~\n");
				int if_cot = 0;

				if_cot = -8*(j+1) ;
				sprintf(if_cot_str, "%d", if_cot);
				strcat(if_cot_str, "(%rbp), ");
				int len = strlen(if_cot_str);
				debug("strlen(if_cot_str) : %d\n",len);
				if_cot_str[len-1] = '\0';

				debug("if_cot_str : %s\n",if_cot_str);

				rewrite(asmFileName, "\tmov",if_cot_str, "%eax", "");
				if_cot_str[0] = '\0';
			}else{

				if(strMember(if_stmt[1][0], DIGIT)){

					strcpy(if_cot_str, "$");
					strcat(if_cot_str, if_stmt[1]);
					strcat(if_cot_str, ",");
					rewrite(asmFileName, "\tmov",if_cot_str, "%eax","");
					if_cot_str[0] = '\0';
				}else if( if_stmt[1][0] == '.'  ){

					int temp_num;
					char temp_num_str[20];
					debug("if_op_star\n");

					sprintf(temp_num_str, "%s", if_stmt[1]+2);
					debug("temp_num_str : %s\n", temp_num_str);
					temp_num = atoi(temp_num_str);	
					debug("temp_num : %d\n", temp_num);
					int_arith(asmFileName, temp_num);

				}else{
					sprintf(if_cot_str, "%s", if_stmt[1]);
					strcat(if_cot_str, "(%rip), ");
					int len = strlen(reg_cot_str);
					debug("strlen(if_cot_str) : %d\n",len);
					if_cot_str[len-1] = '\0';

					debug("if_cot_str : %s\n",if_cot_str);

					rewrite(asmFileName, "\tmov",if_cot_str, "%eax","");
					if_cot_str[0] = '\0';

				}	

			}

			rewrite(asmFileName, "\tpush", "%rax", "\t# op_if_1095 ",""  );
			if(k != -1){


				debug("if_stmt[3] confirm~\n");
				int if_cot = 0;

				if_cot = -8*(k+1) ;
				sprintf(if_cot_str, "%d", if_cot);
				strcat(if_cot_str, "(%rbp), ");
				int len = strlen(if_cot_str);
				debug("strlen(if_cot_str) : %d\n",len);
				if_cot_str[len-1] = '\0';

				debug("if_cot_str : %s\n",if_cot_str);

				rewrite(asmFileName, "\tmov",if_cot_str, "%eax", "");
				if_cot_str[0] = '\0';
			}else{

				if(strMember(if_stmt[3][0], DIGIT)){

					strcpy(if_cot_str, "$");
					strcat(if_cot_str, if_stmt[3]);
					strcat(if_cot_str, ",");
					rewrite(asmFileName, "\tmov",if_cot_str, "%eax", "");
					if_cot_str[0] = '\0';
				}else if( if_stmt[3][0] == '.'  ){

					int temp_num;
					char temp_num_str[20];
					debug("if_op_star\n");

					sprintf(temp_num_str, "%s", if_stmt[3]+2);
					debug("temp_num_str : %s\n", temp_num_str);
					temp_num = atoi(temp_num_str);	
					debug("temp_num : %d\n", temp_num);
					int_arith(asmFileName, temp_num);

				}else{
					sprintf(if_cot_str, "%s", if_stmt[3]);
					strcat(if_cot_str, "(%rip), ");
					int len = strlen(reg_cot_str);
					debug("strlen(if_cot_str) : %d\n",len);
					if_cot_str[len-1] = '\0';

					debug("if_cot_str : %s\n",if_cot_str);

					rewrite(asmFileName, "\tmov",if_cot_str, "%eax", "");
					if_cot_str[0] = '\0';
				}
			}

			rewrite(asmFileName, "\tpop", "%rcx", "", "" );
			rewrite(asmFileName, "\tcmp", "%rax,", "%rcx", "" );
			if(strEqual(if_stmt[2], "==" )){
				rewrite(asmFileName, "\tsete", "%al", "",""  );
			}else if(strEqual(if_stmt[2], "<=" )){
				rewrite(asmFileName, "\tsetle", "%al", "",""  );
			}else if(strEqual(if_stmt[2], ">=" )){
				rewrite(asmFileName, "\tsetge", "%al", "",""  );
			}else if(strEqual(if_stmt[2], "<" )){
				rewrite(asmFileName, "\tsetl", "%al", "",""  );
			}else if(strEqual(if_stmt[2], ">" )){
				rewrite(asmFileName, "\tsetg", "%al", "",""  );

			}
			rewrite(asmFileName, "\tmovzb", "%al,", "%eax",""  );

		}else{
			char local_dec_sum[200];
			char local_dec[20];
			debug("dec_str[0] : %s\n",dec_str[0]);
			debug("dec_num : %d\n",dec_num);
			local_dec_sum[0] = '|';
			local_dec_sum[1] = '\0';
			for(i=0;i<dec_num;i++){

				debug("dec_title_str[%d] : %s\n", i, dec_title_str[i] );
				strncpy(local_dec,dec_title_str[i],20 );
				int len = strlen(local_dec);
				debug("strlen(local_dec) : %d\n",len);
				local_dec[len] = '\0';
				debug("local_dec : %s\n",local_dec);
				strcat(local_dec_sum, local_dec);
				strcat(local_dec_sum, "|");
			}
			debug("local_dec_sum : %s\n",local_dec_sum);
			debug("p1 : %s\n",p1);
			char if_cot_str[20]; 
			if(! strPartOf(p1, local_dec_sum)  ){
				debug("global op_if op_arg!!\n");  	



				sprintf(if_cot_str, "%s", p1);
				strcat(if_cot_str, "(%rip), ");
				int len = strlen(if_cot_str);
				debug("strlen(if_cot_str) : %d\n",len);
				if_cot_str[len-1] = '\0';

				debug("if_cot_str : %s\n",if_cot_str);

				rewrite(asmFileName, "\tmov", if_cot_str, "%eax","");
				if_cot_str[0] = '\0';


			}else{
				debug("local op_arg~\n");
				int if_cot = 0;
				if_cot -= 8;
				sprintf(if_cot_str, "%d", if_cot);
				strcat(if_cot_str, "(%rbp), ");
				int len = strlen(if_cot_str);
				debug("strlen(if_cot_str) : %d\n",len);
				if_cot_str[len-1] = '\0';
				debug("if_cot_str : %s\n",if_cot_str);
				rewrite(asmFileName, "\tmov",if_cot_str, "%eax","");
				if_cot_str[0] = '\0';
			}
		}

		debug("if_str : %s\n",if_str);
		rewrite(asmFileName, "\ttest", "%rax,", "%rax","");
		if(je_use == 1){
			rewrite(asmFileName, "\tje", pTo, "","");
		}else{
			rewrite(asmFileName, "\tjne", pTo, "","");
		}
	}else if(strEqual(op, "op_add")){

		rewrite(asmFileName, "\t", "LD", p1, p2);
	}else if(strEqual(op, "op_call")){
		debug("arg_mask %lld\n", arg_mask);
		debug("reg_mask %lld\n", reg_mask);
		debug("arg_num %d\n", arg_num);

		int gv=0, pv=0, lv=0;
		char Arithmetic_token[20];
		int Arithmetic_token_num;

		Arithmetic_num = 0;
		current_n = &linklistTable[Arithmetic_num];

		while(current_n->rlink != NULL){
			current_n = current_n->rlink;
			if(current_n->num[0] == '.'){
				if(ptr->rlink!= NULL){
					pv += 1;
				}else{
					gv += 1;
				}
			}else{
				if(current_n->score == -1){
					gv += 1;
				}else{
					lv += 1;
				}
			}
		}

		debug("gv = %d, pv = %d, lv = %d\n", gv, pv, lv);

		int plv = 0;
		int gplv = 0;
		plv = pv + lv;
		gplv = gv + pv +lv;

		for( i = 0; i <gplv ; i++ ){
			push(asmFileName, REGS[i], "\t# AST_FUNCALL_1135");
		}

		for(i=0; i<arg_num; i++){
			if( (arg_mask & (1 << (i))) != 0  ){
				debug("arg_str[%d] = %s \n", i, arg_str[i]);

				rewrite(asmFileName, "\tlea", arg_str[i], "%rax", "" );
				rewrite(asmFileName, "\tpush %rax", "", "\t# op_call_1261","" );
			}else{

				debug("reg_str[%d] = %s \n", i, reg_str[i]);
				rewrite(asmFileName, "\tmov",reg_str[i], "%eax","");
				rewrite(asmFileName, "\tpush", "%rax", "\t# op_call_1273","" );
			}
		}

		for( i = arg_num-1; i >=0; i-- ){
			rewrite(asmFileName, "\tpop", REGS[i], "\t# op_call_1281","");
		}

		rewrite(asmFileName, "\tmov", "$0,", "%eax","" );
		rewrite(asmFileName, "\tcall", p1, "","" );

		for (i = arg_num-1; i>0; i--){

			rewrite(asmFileName, "\tpop", REGS[i], "\t# op_call_1290","" );
		}
		arg_num = 0;
		reg_cot = 0;

	}else if(strEqual(op, "op_arg")){
		arg_num += 1;
		if(strHead(p1, ".") ){
			if( (reg_mask & (1 << (arg_num-1) ) ) == 1 ){ debug("op_Arg arg_mask error !! \n"); }
			arg_mask |= 1 << (arg_num-1);


			strcat(p1, "(%rip),");
			strcpy(arg_str[arg_num-1], p1);


		}else{
			if( (arg_mask & (1 << (arg_num-1) ) ) == 1 ){ debug("op_Arg reg_mask error !! \n"); }

			char local_dec_sum[200];
			char local_dec[20];
			debug("dec_str[0] : %s\n",dec_str[0]);
			debug("dec_num : %d\n",dec_num);
			local_dec_sum[0] = '|';
			local_dec_sum[1] = '\0';
			for(i=0;i<dec_num;i++){

				debug("dec_title_str[%d] : %s\n", i, dec_title_str[i] );
				strncpy(local_dec,dec_title_str[i],20 );
				int len = strlen(local_dec);
				debug("strlen(local_dec) : %d\n",len);
				local_dec[len] = '\0';
				debug("local_dec : %s\n",local_dec);
				strcat(local_dec_sum, local_dec);
				strcat(local_dec_sum, "|");
			}
			debug("local_dec_sum : %s\n",local_dec_sum);
			debug("p1 : %s\n",p1); 
			if(! strPartOf(p1, local_dec_sum)  ){
				debug("global op_arg!!\n");  	

				reg_mask |= 1 << (arg_num-1);

				sprintf(reg_cot_str, "%s", p1);
				strcat(reg_cot_str, "(%rip), ");
				int len = strlen(reg_cot_str);
				debug("strlen(reg_cot_str) : %d\n",len);
				reg_cot_str[len-1] = '\0';
				strcpy(reg_str[arg_num-1], reg_cot_str);
				debug("reg_cot_str : %s\n",reg_cot_str);
				debug("reg_str[%d] = %s \n",arg_num-1,reg_str[arg_num-1] );
				reg_cot_str[0] = '\0';

			}else{debug("local op_arg~\n");

				reg_mask |= 1 << (arg_num-1);
				reg_cot -= 8;
				sprintf(reg_cot_str, "%d", reg_cot);
				strcat(reg_cot_str, "(%rbp), ");
				int len = strlen(reg_cot_str);
				debug("strlen(reg_cot_str) : %d\n",len);
				reg_cot_str[len-1] = '\0';
				strcpy(reg_str[arg_num-1], reg_cot_str);
				debug("reg_cot_str : %s\n",reg_cot_str);
				debug("reg_str[%d] = %s \n",arg_num-1,reg_str[arg_num-1] );

				reg_cot_str[0] = '\0';
			}
		}
	}else if(strEqual(op, "op_dec")){

		if(strHead(p1, ".") ){
			strcpy( if_stmt[0], p1 );
			debug("if_stmt[0] : %s\n",if_stmt[0]);	
		}else{

			dec_num += 1;
			dec_state = 1;
			dec_temp_str[0] = '\0';	
			strcat(dec_temp_str,p1);
			int len = strlen(dec_temp_str);
			debug("strlen(dec_temp_str) : %d\n",len);
			dec_temp_str[len] = '\0'; 
			debug("dec_temp_str : %s \n",dec_temp_str);

			strcpy( dec_title_str[dec_num-1], dec_temp_str );  
			debug("dec_title_str[%d] : %s\n", dec_num-1, dec_title_str[dec_num-1]);

			debug("p2 : %s\n", p2);
			strcat(mov_str,p2);
			len = strlen(mov_str);
			debug("strlen(mov_str) : %d\n",len);
			mov_str[len] = ','; 
			mov_str[len+1] = '\0'; 

			strcpy( dec_str[dec_num-1], mov_str );  
			debug("dec_str[%d] : %s\n", dec_num-1, dec_str[dec_num-1]);
			mov_str[6] = '\0'; 
		}
	}else{
		int temp_num;
		char temp_num_str[20];
		debug("op_star\n");

		debug("p2[0] : %c\n", p2[0]);

		if( p2[0] == '*'  ){
			sprintf(temp_num_str, "%s", p2+3);


			sprintf(Arithmetic[0], "%s", p2+3);
			debug("Arithmetic[0] : %s\n", Arithmetic[0]);
			Arithmetic_num = atoi(Arithmetic[0]);
			linklist_add_star(Arithmetic_num);

		}else{
			sprintf(temp_num_str, "%s", p2+2);
		}

		debug("temp_num_str : %s\n", temp_num_str);
		temp_num = atoi(temp_num_str);	
		debug("temp_num : %d\n", temp_num);
		int_arith(asmFileName, temp_num);

		int temp_switch = -1;

		for(i=0;i<dec_num;i++){

			debug("dec_title_str[%d] : %s\n", i, dec_title_str[i] );
			if(strEqual(op, dec_title_str[i])){
				debug("op & dec_title_str[i] find! \n");
				temp_switch = i;
			}
		}

		if( temp_switch == -1  ){

			rewrite(asmFileName, "\t# mov", "(%rax),", "%rcx", "\t# op_none_1454" );
			rewrite(asmFileName, "\t# mov", "%ecx,", "(%rax)", "\t# op_none_1455" );

			if( p1[0] == '=' ){
				sprintf(temp_num_str, "%s(%%rip)", op);
				debug("temp_num_str : %s\n", temp_num_str);

				rewrite(asmFileName, "\t# mov", "%eax,", temp_num_str, "\t# op_none_1462" );
			}
		}else{
			dec_cot = temp_switch *-8;
			dec_cot -= 8;
			sprintf(dec_cot_str, "%d", dec_cot);
			strcat(dec_cot_str, "(%rbp) ");
			rewrite(asmFileName, "\tmov", "%eax,", dec_cot_str, "\t# op_none_1471");
		}
	}
}


void fileToLine(char *fileName, char* asmFileName ){

	FILE *fp; 

	char op[32];
	char p1[32];
	char p2[32];
	char pTo[32];
	char pToo[32];
	char StrLine[1024];

	static int lineNum = 1;
	static int size = 50;

	if((fp = fopen(fileName,"r")) == NULL) 
	{ 
		printf("file open error!"); 
		return; 
	} 

	while( !feof(fp)  ){
		fgets(StrLine, size, fp);
		splitLineStr(StrLine, op, p1, p2, pTo, pToo );
		pcodeToAsm(asmFileName, op, p1, p2, pTo, pToo );
	}
	fclose(fp);
	return;
}


void fileToStr(char *fileName, char *asmFileName){

		fileToLine(fileName,  asmFileName);
}


void compile(char *cFile, char *asmFileName){

        debug("compile file:%s \n", cFile);

        fileToStr(cFile, asmFileName);

        rewrite(asmFileName, "\tleave","", "", "");
        rewrite(asmFileName, "\tret","", "", "");
}


int main(int argc, char *argv[]){

	char *cFile;
	char *asmFile;

	if(argc == 3){
		cFile = argv[1];
		asmFile = argv[2];
	}else{
		printf("usage: ./lugen test.or test.s \n");
		return 0;
	}

	compile(cFile, asmFile);

	return 0;
}


