#include "proc.h"
#include "fs.h"

#define DEFAULT_ENTRY 0x4000000

// 从ramdisk中`offset`偏移处的`len`字节读入到`buf`中
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
extern size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  uint32_t len = fs_filesz(fd);
  uint32_t blen = 1024;
  uintptr_t s = DEFAULT_ENTRY;
  // // ramdisk_read(buf, 0, len);
  Log("loader: len 0x%x\n", len);
  char buf[blen];
  while (len > blen) {
    fs_read(fd, buf, blen);
    memcpy((void *)s, buf , blen);
    s += blen;
    len -= blen;
  }
  fs_read(fd, buf, len);
  memcpy((void *)s, buf , len);
  fs_close(fd);

  // Log("file length: 0x%x\n", len);
  // fs_read(fd, buf, len);
  
  // memcpy((void *)s, buf , len);
  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
