#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  if (free_ == NULL) {
    printf("No free watchpoints.\n");
    return NULL;
  }
  WP *ret = free_;
  free_ = free_->next;

  ret->next = head;
  head = ret;

  return ret;
}
void free_wp(WP *wp) {
  WP *pW, *W;
  pW = head;
  if (pW->NO == wp->NO) head = pW->next;
  else while (pW->next != NULL) {
      W = head->next;
      if (W->NO == wp->NO) {
        pW->next = W->next;
        return;
      }
    }
  wp->next = free_;
  free_ = wp;
  return;
}

