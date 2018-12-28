#include "proc.h"

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current, *fg_pcb;
PCB *pcbs[MAX_NR_PROC];
static int count = 0;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  // naive_uload(&pcb_boot, "/bin/init");
  // context_kload(&pcb[0], (void *)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  // pcb_boot = pcb[1];
  pcbs[0] = &pcb[0];
  pcbs[1] = &pcb[1];
  pcbs[2] = &pcb[2];
  fg_pcb = &pcb[1];
  switch_boot_pcb();
}

_Context* schedule(_Context *prev) {
  current->cp = prev;
  current = fg_pcb;
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  if (current == fg_pcb) {
    count++;
    current = fg_pcb;
  }
  if (count == 100) {
    count = 0;
    current->cp = prev;
    current = &pcb[0];
  }
  
  // count++;
  // current->cp = prev;
  // current = &pcb[1];
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  

  return current->cp;
}
