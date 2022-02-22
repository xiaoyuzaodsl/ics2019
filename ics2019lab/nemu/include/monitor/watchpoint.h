#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
enum { wbreakpoint, wwatchpoint };
enum { wkeep, wdel,wdis};
enum {wyes,wno};
typedef struct watchpoint {
  int NO;
  int type;
  int disp;
  int enable;
  int address;
  char what[32];
  char expr[128];
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;
WP* new_wp();
void free_wp(WP *wp); 
void list_wp();
int free_wp_by_num(int n);
int cal();
void print_wp_info(WP *tp);
//int wp_expr_cal[32][2];
#endif
