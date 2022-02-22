#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32
static char my_type[2][15] = {"breakpoint", "watchpoint"};
static char my_disp[3][10] = {"keep","del","dis"};
static char my_enbl[2][10] = {"yes","no"};
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
/*head---->正在使用*/
/*free_---->空闲*/
WP* new_wp();
void free_wp(WP *wp); //释放监视点
int free_wp_by_num();
//初始化
void print_wp_info(WP *tp){
	//printf("Num Type        Disp    Enb Address     What            Expr\n");
	printf("%d	%s	%s  %s  %x  %s      %s\n",
			tp->NO,my_type[tp->type],my_disp[tp->disp],
			my_enbl[tp->enable],tp->address,tp->what,tp->expr);  
}
void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
	wp_pool[i].type = wbreakpoint;
	wp_pool[i].disp = wkeep;
	wp_pool[i].enable = wyes;
	wp_pool[i].address = 0;
	wp_pool[i].what[0] = '\0';
	wp_pool[i].expr[0] = '\0';
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

//
WP* new_wp(){
	printf("新建了一个监视点：\n");
	WP *me =  free_;
	WP *prev = free_;
	if(me==NULL){ 
		printf("No free watchpoint\n");
		return NULL;
	}
	while(me->next!=NULL){
		prev = me;
		me = me->next;
	}
//	printf("prev num:%d		me num:%d\n",prev->NO,me->NO);
	prev->next = NULL;
	me->next = NULL;
	WP *head_wp_tail = head;
	if(head_wp_tail==NULL){
		head = me;
//		printf("现在头是空的\n");
//		printf("head num: %d\n",head->NO);
//		printf("me num:%d\n",me->NO);
		return me;
	}
	while(head_wp_tail->next!=NULL){
		head_wp_tail = head_wp_tail->next;
	}
//	printf("头不是空的\n");
//	printf("head_wp_tail num: %d\n",head_wp_tail->NO);
	head_wp_tail->next = me;
//	printf("me num:%d\n",me->NO);
	return me;
}
void free_wp(WP *wp){
	printf("清除监视点\n");
	if(wp==NULL){
		printf("illegal watchpoint\n");
		return;
	}
	WP *me = head;
	WP *prev = head;
	while(me!=wp && me!=NULL){
		prev = me;
		me = me->next;
	}
	if(me==NULL){
		printf("no such watchpoint,please remove a legal one\n");
		return;
	}
	if(head==wp){
		me = head;
		head = head->next;
		me->next = NULL;
		prev = free_;
		WP *tmp = free_;
		while(tmp!=NULL){
			prev = tmp;
			tmp = tmp->next;
		}
		prev->next = me;
		return;
	}
	prev->next = me->next;
	me->next = NULL;
	prev = free_;
	WP *tmp = free_;
	while(tmp!=NULL){
		prev = tmp;
		tmp = tmp->next;
	}
	prev->next = me;
	return;
}
void list_wp(){
	WP *tp = head;
	if(head==NULL){ 
		printf("No breakpoints\n");
		return;
	}
	printf("Num	Type		Disp	Enb	Address		What			Expr\n");
	while(tp!=NULL){
		print_wp_info(tp);
		tp = tp->next;
	}
}

int free_wp_by_num(int n){
	WP *tmp = head;
	while(tmp!=NULL){
		if(tmp->NO==n)break;
		else tmp = tmp->next;
	}
	if(tmp==NULL) printf("No such point, error\n");
	else{
		free_wp(tmp);
	}
	return 0;
}
int cal(){
	WP *tmp = head;
	if(head==NULL) return 0;
	int flag = 0;
	while(tmp!=NULL){
		int res = expr(tmp->expr,0);
		if(res!=tmp->address){
			if(flag==0){
				printf("注意！监视点发生变化：\n");
				printf("Num	Type		Disp	Enb	Address		What			Expr\n");
			}
			print_wp_info(tmp);
			printf("第%d号监视点从 0x%x -> 0x%x(二进制)\n",tmp->NO,tmp->address,res);	
			flag++;
		}
		tmp->address = res;
		tmp = tmp->next;
	}
	return flag;
}
