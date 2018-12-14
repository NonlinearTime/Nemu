#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expression[1024];
  uint32_t old_value;


} WP;

WP* new_wp();

void free_wp(WP *wp);

#endif
