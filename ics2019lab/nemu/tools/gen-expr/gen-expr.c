#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536 - 1024];
/*以下的几个函数生成表达式，结果输出到buf字符串*/
static inline int choose(int n){
	//int ch_seed = time(0);
	//srand(ch_seed);
	int res = rand();
	if(res < 0) res = -res;
   	res = res % n;
	return res;
}

static inline void gen_num(){
	char tmp[32];
	memset(tmp,'\0',sizeof(tmp));
	int num = choose(100) + 1;//算了干脆就不生成0这个随机数了
	if(num >= 100) num = 99;
	sprintf(tmp,"%d",num);
	strcat(buf,tmp);
}
static inline void gen(char x){
	char tmp[2];
	tmp[0] = x; tmp[1] = '\0';
	strcat(buf,tmp);
}
static inline void gen_rand_op(){
	int choice = choose(4);
	char tmp[2];  tmp[0] = '\0';  tmp[1] = '\0';
	switch(choice){
		case 0:
			tmp[0] = '+'; break;
		case 1:
			tmp[0] = '-'; break;
		case 2:
			tmp[0] = '*'; break;
		case 3:
			//除法，希望除数不为0
			//tmp[0] = '/'; break;
			tmp[0] = '+'; break;
	}
	strcat(buf,tmp);
}

static inline void gen_rand_expr() {
  //buf[0] = '\0';
  switch (choose(3)) {
    case 0: 
		gen_num(); 
		break;
    case 1: 
		gen('('); 
		gen_rand_expr(); 
		gen(')'); 
		break;
    default: 
		gen_rand_expr(); 
		gen_rand_op(); 
		gen_rand_expr(); 
		break;
  }
}

/*生成特定长度的表达式*/
/*
static inline void gen_rand_expr(int len) {
	if(len<=1) return;
	if(len==2) gen_num();
	if(len%2==0) len++;
	int x = 0;
	switch (choose(3)) {
		//case 0: 
			//gen_num(); 
			//break;
		case 1: 
			gen('('); 
			gen_rand_expr(len - 2); 
			gen(')'); 
			break;
		default: 
			x = (len-1)/ 2;
			gen_rand_expr(x); 
			gen_rand_op(); 
			gen_rand_expr(len - 1 - x); 
			break;
	}
}
*/
static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    memset(buf,'\0',sizeof(buf));
	gen_rand_expr(30);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if(fscanf(fp, "%d", &result)==EOF) return 0;
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
