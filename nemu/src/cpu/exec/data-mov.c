#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  interpret_rtl_lm(&t0, &cpu.esp, 4);
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_addi(&reg_l(R_ESP), &reg_l(R_EBP), 0);    // only implement for 32bit 
  rtl_pop(&reg_l(R_EBP));
  // printf("0x%x\n", reg_l(R_EBP));
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_msb(&t0, (uint32_t*)&reg_w(R_EAX), 2);
    if (t0) reg_w(R_EDX) = 0xffff;
    else reg_w(R_EDX) = 0;
  }
  else {
    rtl_msb(&t0, (uint32_t*)&reg_l(R_EAX), 4);
    if (t0) reg_l(R_EDX) = 0xffffffff;
    else reg_l(R_EDX) = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_msb(&t0, (uint32_t*)&reg_b(R_AL), 1);
    if (t0) reg_b(R_AH) = 0xff;
    else reg_b(R_AH) = 0;
  }
  else {
    rtl_msb(&t0, (uint32_t*)&reg_w(R_AX), 2);
    if (t0) reg_l(R_EAX) = 0xffff0000 | reg_w(R_AX);
    else reg_l(R_EAX) = reg_w(R_AX);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
