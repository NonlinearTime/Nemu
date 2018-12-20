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
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

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
    rtl_msb(&t0, &id_dest->val, 2);
    if (t0) reg_w(R_EDX) = 0xffff;
    else reg_w(R_EDX) = 0;
  }
  else {
    rtl_msb(&t0, &id_dest->val, 4);
    if (t0) reg_l(R_EDX) = 0xffffffff;
    else reg_l(R_EDX) = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_msb(&t0, &id_dest->val, 1);
    if (t0) reg_b(R_EDX) = 0xff;
    else reg_b(R_EDX) = 0;
  }
  else {
    rtl_msb(&t0, &id_dest->val, 2);
    if (t0) reg_w(R_EDX) = 0xffff;
    else reg_w(R_EDX) = 0;
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
