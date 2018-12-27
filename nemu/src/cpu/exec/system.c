#include "cpu/exec.h"
#include "device/port-io.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
  // interpret_rtl_host_lm(&cpu.idtr.limit, id_dest->val, 2);
  // interpret_rtl_host_lm(&cpu.idtr.base, id_dest->val + 16, 4);
  cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4);
#ifdef DEBUG
  // printf("lidt: 0x%x 0x%x\n", id_dest->addr, cpu.idtr.base);
#endif
  
  if (decoding.is_operand_size_16) {
    cpu.idtr.base &= 0x00ffffff;
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  switch(id_dest->reg) {
    case 0: cpu.cr0.val = id_src->val; break;
    case 1: cpu.cr1 = id_src->val; break;
    case 2: cpu.cr2 = id_src->val; break;
    case 3: cpu.cr3.val = id_src->val; break;
    default: assert(0);
  }

  print_asm("movl %%%s,%%cr%d %d", reg_name(id_src->reg, 4), id_dest->reg, id_src->reg);
}

make_EHelper(mov_cr2r) {
  switch(id_src->reg) {
    case 0: t0 = cpu.cr0.val; break;
    case 1: t0 = cpu.cr1; break;
    case 2: t0 = cpu.cr2; break;
    case 3: t0 = cpu.cr3.val; break;
    default: assert(0);
  }
  operand_write(id_dest, &t0);
  
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));
  print_asm("movl %%cr%d,%%%s %d", id_src->reg, reg_name(id_dest->reg, 4), t0);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  raise_intr(id_dest->val, *eip);

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  rtl_pop(&t0);
  // printf("iret: 0x%x\n", *eip);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags);
  rtl_jr(&t0);

  print_asm("iret");
}

make_EHelper(in) {  
  switch(id_src->width) {
    case 4:
      t0 = pio_read_l(id_src->val);
      break;
    case 1:
      t0 = pio_read_b(id_src->val);
      break;
    case 2:
      t0 = pio_read_w(id_src->val);
      break;
    default: assert(0);
  }
  
  operand_write(id_dest, &t0);

  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  operand_write(id_dest, &id_src->val);

  switch(id_src->width) {
    case 4: pio_write_l(id_dest->val, id_src->val); break;
    case 1: pio_write_b(id_dest->val, id_src->val); break;
    case 2: pio_write_w(id_dest->val, id_src->val); break;
    default: assert(0);
  }

  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
