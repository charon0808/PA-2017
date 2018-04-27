#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct  watchpoint{
  int NO;
  struct watchpoint *next;
  char args[128];
  int old_value;
  /* TODO: Add more members if necessary */


} WP;

extern bool hit_wp();
extern WP* new_up();
extern bool free_wp();
extern void print_wp();

#endif
