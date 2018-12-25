#include "common.h"
#include "syscall.h"
#include "fs.h"

uintptr_t sys_yield();
void sys_exit(int code);
size_t sys_write(int fd, void *buf, size_t count);
size_t sys_brk(void * addr);
int sys_open(const char *pathname, int flags, int mode);
ssize_t sys_read(int fd, void *buf, size_t len);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_close(int fd);

intptr_t program_brk;

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // Log("do_syscall: %d %d %d %d\n", c->GPR1, c->GPR2, c->GPR3, c->GPR4);

  switch (a[0]) {
    case SYS_yield: c->GPR1 = sys_yield(); break;
    case SYS_exit: sys_exit(a[1]); break;
    case SYS_write: c->GPR1 = sys_write(a[1], (void *)a[2], a[3]); break;
    case SYS_brk: c->GPR1 = sys_brk((void *)a[1]); break;
    case SYS_open:  c->GPR1 = sys_open((char *)a[1], a[2], a[3]) ; break;
    case SYS_read:  c->GPR1 = sys_read(a[1], (void *)a[2], a[3]) ; break;
    case SYS_close: c->GPR1 = sys_close(a[1]) ; break;
    case SYS_lseek:  c->GPR1 = sys_lseek(a[1], a[2], a[3]); break;
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
  return fs_write(fd, buf, count);
}

size_t sys_brk(void * addr) {
  Log("sys_brk: addr %d\n", addr);
  program_brk = (intptr_t)addr;
  Log("sys_brk: new_program_brk %d\n", addr);
  return 0;
}

int sys_open(const char *pathname, int flags, int mode) {
  return fs_open(pathname, flags, mode);
}

ssize_t sys_read(int fd, void *buf, size_t len) {
  return fs_read(fd, buf, len);
}
off_t sys_lseek(int fd, off_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}
int sys_close(int fd) {
  return fs_close(fd);
}