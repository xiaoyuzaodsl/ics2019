#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
extern void isa_reg_display(void);
extern uint32_t expr(char *e, bool *success);
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
static int cmd_si(char *args){
	int stp = 0;
	if(args==NULL) stp = 1;
	else sscanf(args,"%d",&stp);
	//printf("stp = %d\n",stp);
	cpu_exec(stp);
	return 0;
}
static int cmd_info(char *args){
	char ord = 'r';
	if(args==NULL){
		ord = 'x';
	}
	else sscanf(args,"%c",&ord);
	//printf("ord = %c\n",ord);
	if(ord=='r'){
		printf("ord = r\n");
		isa_reg_display();
	}
	else if(ord=='w'){
		printf("ord = w\n");
		list_wp();
	}
	else printf("Uknown order\n");
	return 0;
}
static int cmd_x(char *args){
	int len = 0;
	uint32_t st = 0;
	sscanf(args,"%d %x",&len,&st);
	if(len%4==0){
		len = len;
	}
	else len = (len / 4 + 1) * 4;
	printf("len = %d and st = 0x%x\n",len,st);
	int xxx = len / 4;
	while(xxx--){
		printf("0x%x : 0x%x\n",st,vaddr_read(st,len));
		st+=4;
	}
	return 0;
}
static int cmd_p(char *args){
	bool *suc = 0;
	expr(args,suc);
	return 0;
}
static int cmd_help(char *args);
static int cmd_b(char *args){
	//printf("cmd_b\n");
	WP* me = new_wp();
	//printf("wew_wp success\n");
	me->type = wbreakpoint;
	me->disp = wkeep;
	me->address = expr(args,0);/*???*/
	me->enable = wyes;
	strcpy(me->what,"not sure now");
	strcpy(me->expr,args);
	print_wp_info(me);
	if(me==NULL) printf("注意！生成监视点失败！\n");
	return 0;
}
static int cmd_d(char* args){
	int num = 0;
	sscanf(args,"%d",&num);
	free_wp_by_num(num);
	return 0;
}
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "单/多步执行",cmd_si},
  { "info","打印程序状态: r打印寄存器，w打印监视点",cmd_info},
  { "x","扫描内存",cmd_x},
  { "p","表达式计算",cmd_p},

  { "b","添加断点",cmd_b},
  { "w","添加监视点(本质与添加断点一致)",cmd_b},
  { "d","删除监视点",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
