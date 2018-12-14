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
void free_wp(int n) {
  WP *pW, *W;
  pW = head;
  W = pW->next;
  if (pW->NO == n) head = pW->next;
  else while (W != NULL) {
      if (W->NO == n) {
        pW->next = W->next;
        return;
      }
      pW = W;
      W = W->next;
    }
  wp_pool[n].next = free_;
  free_ = &wp_pool[n];
  return;
}

WP* get_wp_head() {
  return head;
}

WP* get_free_head() {
  return free_;
}

WP* check_watchpoints() {
  WP *wp = head;
  bool success;
  uint32_t res;
  while (wp != NULL) {
    res = expr(wp->expression, &success);
    if (res != wp->old_value) {
      wp->old_value = res;
      return wp;
    }
    wp = wp->next;
  }
  return wp;
}

WP* print_watchpoints() {
  WP *wp = head;
  while (wp != NULL) {
    printf("%d %s %u\n", wp->NO, wp->expression, wp->old_value);
    wp = wp->next;
  }
  return wp;
}
