#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  t0 = cpu.idtr.base;
  t1 = t0 + 8 * NO;
  t0 = vaddr_read(t1, 2);
  t2 = vaddr_read(t1 + 32, 4);
  t1 = t0 + (t2 & 0xffff0000);
  rtl_jr(&t1);
}

void dev_raise_intr() {
}
