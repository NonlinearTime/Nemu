#include "common.h"
#include "syscall.h"

uintptr_t sys_yield();

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  // printf("%d\n", c->GPR1);

  switch (a[0]) {
    case SYS_yield: c->GPR1 = sys_yield(); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

uintptr_t sys_yield() {
  _yield();
  return 0;
}

