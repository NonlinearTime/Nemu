#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // printf("raise_intr: 0x%x 0x%x 0x%x\n", cpu.eflags, cpu.cs, ret_addr);
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  NO = NO - 1;
  t0 = cpu.idtr.base;
  t1 = t0 + 8 * NO;
  printf("raise_intr: 0x%x 0x%x 0x%x\n",NO, t0, t1);
  #ifdef DEBUG
  // printf("raise_intr: 0x%x 0x%x 0x%x\n", cpu.eflags, cpu.cs, ret_addr);
  #endif
  t0 = vaddr_read(t1, 2);
  t2 = vaddr_read(t1 + 4, 4);
  t1 = t0 + (t2 & 0xffff0000);
  printf("raise_intr: 0x%x 0x%x\n", t0, t1);
  rtl_jr(&t1);
}

void dev_raise_intr() {
}
