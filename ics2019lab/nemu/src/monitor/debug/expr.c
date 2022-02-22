#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
extern uint32_t isa_reg_str2val(const char *s, bool *success);
enum {
  TK_NOTYPE = 256, TK_EQ,NUMBER,
  HEX,REG,TK_NEQ,
  AND,OR,
  DEREF,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-",'-'},
  {"\\*",'*'},
  {"/",'/'},
  {"\\(",'('},
  {"\\)",')'},

  {"==", TK_EQ},         // equal
  {"0x[0-9a-f]+",HEX},

  {"[0-9]+",NUMBER},

  //{"0x[0-9]+",HEX},
  {"[\\$]e[a,b,c,d]x",REG},{"[\\$][a,b,c,d]x",REG},
  {"[\\$][a,b,c,d]h",REG},{"[\\$][a,b,c,d]l",REG},
  {"!=",TK_NEQ},
  {"&&",AND},
  {"[\\|]{2}",OR},
  //{"*"},这里表示的是指针
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {

  int type;
  char str[32];
} Token;

static Token tokens[256] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;
int numb = 0;
static bool make_token(char *e) {
  int position = 0;
  int i;
  numb = 0;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",i, rules[i].regex, position, substr_len, substr_len, substr_start);
        //position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		char tm[32];
        switch (rules[i].token_type) {
          default: //TODO();
			  strncpy(tm,e+position,substr_len);
			  tm[substr_len] = '\0';
			  tokens[numb].type = rules[i].token_type;
			  strcpy(tokens[numb].str,tm);
			  numb++;
		      break;
        }
		position += substr_len;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
bool check_parentheses(int p, int q){
	if(tokens[p].type=='(' && tokens[q].type==')'){
		int pth = 0;
		for(int i = p + 1;i < q;i++){
			if(tokens[i].type=='(') pth++;
			else if(tokens[i].type==')') pth--;
			if(pth < 0){
				return false;
			}
		}
		if(pth==0){ 
			//printf("check parentheses true,(  )\n");
			return true;
		}
		if(pth > 0 || pth < 0) return false;
	}
	else if(tokens[p].type==NUMBER && tokens[q].type==NUMBER){
		//printf("check parentheses true,no ()\n");
		return false;
	}
	return false;
}
struct lt{
	int pos;
	Token me;
}mt[256];
int find_main_token(int p,int q){
	//if(p > q) return -1;
	//if(p+1==q) return -1;
	int tip = 0;
	//Token mt[32];
	for(int i = p;i <= q;i++){
		/*括号里面的不算，跳过*/
		if(tokens[i].type=='('){
			int fuck = 1;
			i++;
			while(fuck > 0){
				if(tokens[i].type=='(') fuck++;
				else if(tokens[i].type==')') fuck--;
				i++;
			}
			i--;
		}
		/*数字不算，跳过*/
		else if(tokens[i].type==NUMBER) i = i;
		/*带0x的16进制数也不算*/
		else if(tokens[i].type==HEX) i = i;
		/*寄存器不算*/
		else if(tokens[i].type==REG) i = i;
		else if(tokens[i].type==DEREF){
			if(tokens[i+1].type=='(') i = i;//如果是解引用一个括号，不用考虑括号里面
			else i++;//说明解引用的是一个数字或者寄存器，跳过
		}
		/*以防万一右括号也不算*/
		else if(tokens[i].type==')')i = i;
		
		/*剩下的是加减乘除*/
		/*经过扩展之后还有&&,==,!=*/
		/*算数优先级:
		 *&&
		 *==与!=
		 * *与/
		 * +与-
		 * */
		/*主运算符的优先级最低*/
		else{
			(mt[tip].me).type = tokens[i].type;
			strcpy((mt[tip].me).str,tokens[i].str);
			mt[tip].pos = i;
			tip++;
		}
	}
	/*
	for(int i = 0;i < tip;i++){
		printf("main token may be %s at position %d\n",(mt[i].me).str,mt[i].pos);
	}
	*/
	/*  +,-  */
	for(int i = tip - 1;i >= 0;i--){
		if((mt[i].me).type=='+' || (mt[i].me).type=='-'){
			//printf("main token is %s at position %d\n",(mt[i].me).str,mt[i].pos);
			return mt[i].pos;
		}
	}
	/*  *,/  */
	for(int i = tip - 1;i >= 0;i--){
		if((mt[i].me).type=='*' || (mt[i].me).type=='/'){
			//printf("main token is %s at position %d\n",(mt[i].me).str,mt[i].pos);
			return mt[i].pos;
		}
	}
	/*  ==,!=  */
	for(int i = tip - 1;i >= 0;i--){
		if((mt[i].me).type==TK_EQ || (mt[i].me).type==TK_NEQ){
			//printf("main token is %s at position %d\n",(mt[i].me).str,mt[i].pos);
			return mt[i].pos;
		}
	}

	/*  &&,||  */
	for(int i = tip - 1;i >= 0;i--){
		if((mt[i].me).type==AND || (mt[i].me).type==OR){
			//printf("main token is %s at position %d\n",(mt[i].me).str,mt[i].pos);
			return mt[i].pos;
		}
	}

	//printf("main token is %s at position %d\n",(mt[tip-1].me).str,mt[tip-1].pos);
	return mt[tip-1].pos;
}
uint32_t eval(int p,int q){
	//printf("calculate expression between position %d and %d:\n",p,q);
	//printf("expression:");
	//for(int i = p; i <= q;i++) printf("%s",tokens[i].str);
	//printf("\n");
	if(p > q){
		//invalid
		printf("invalid,bad expression between position %d and %d\n",p,q);
		return -1;
	}
	else if(p==q){
		//single token,a number
		int res = 0;
		bool suc = false;
		/*十进制*/
		if(tokens[p].type==NUMBER) sscanf(tokens[p].str,"%d",&res);
		/*16进制*/
		else if(tokens[p].type==HEX) sscanf(tokens[p].str,"%x",&res);
		/*寄存器*/
		else if(tokens[p].type==REG) res = isa_reg_str2val(tokens[p].str,&suc);
		//printf("single token : %s( = %d) at position %d\n",tokens[p].str,res,p);
		return res;
	}
	else if(tokens[p].type==DEREF){
		int val = eval(p+1,q);
		return vaddr_read(val,4);
	}
	else if(check_parentheses(p, q) == true){
		return eval(p+1,q-1);
	}
	else{
		//
		//只有两边都没有括号的时候才进行正经的计算
		int op = find_main_token(p,q);/*the position of 主运算符 in the token expression*/
		int val1 = eval(p,op - 1);
		int val2 = eval(op + 1,q);
		switch(tokens[op].type){
			case '+' : 
				return val1 + val2;break;
			case '-':
				return val1 - val2;break;
			case '*':
				return val1 * val2;break;
			case '/':
				return val1 / val2;break;
			case TK_EQ:
				return (val1==val2);break;
			case TK_NEQ:
				return (val1!=val2);break;
			case AND:	
				return (val1&&val2);break;
			case OR:
				return (val1||val2);break;
			default:
				assert(0);break;
		}
	}
	return 0;
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  /*
  for(int i = 0;i < numb;i++){
	  printf("tokens[%d] = %s, type = %d\n",i,tokens[i].str,tokens[i].type);
  }
  */
  for (int i = 0; i < numb; i ++) {
	  if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != NUMBER && tokens[i - 1].type != HEX && tokens[i - 1].type!=REG)) ) {
		  tokens[i].type = DEREF;
	  }
  }
  for(int i = 0;i < numb;i++){
	  printf("tokens[%d] = %s, type = %d\n",i,tokens[i].str,tokens[i].type);
  }

  int res = eval(0,numb - 1);
  printf("res = %d(十进制) and 0x%x(二进制)\n",res,res);
  return res;
}
