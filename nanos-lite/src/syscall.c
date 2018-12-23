#include "common.h"
#include "syscall.h"

uintptr_t sys_yield();
void sys_exit(int code);
size_t sys_write(int fd, void *buf, size_t count);


_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("%d\n", c->GPR1);

  switch (a[0]) {
    case SYS_yield: c->GPR1 = sys_yield(); break;
    case SYS_exit: sys_exit(a[1]); break;
    case SYS_write: c->GPR1 = sys_write(a[1], (void *)a[2], a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}

uintptr_t sys_yield() {
  _yield();
  return 0;
}

void sys_exit(int code) {
  _halt(code);
}

size_t sys_write(int fd, void *buf, size_t count) {
  if (fd == 1 || fd == 2) {
    size_t i;
    for (i = 0; i < count; ++i) {
      _putc(*((char *)buf + i));
    }
    return i;
  }
  return 0;
}
