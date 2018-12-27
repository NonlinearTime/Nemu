#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  interpret_rtl_and(&t0, &id_dest->val, &id_src->val);

  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  interpret_rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  rtl_update_ZFSF(&t0, id_dest->width);
  
  print_asm_template2(and);
}

make_EHelper(xor) {
  Log("xor:\n");
  interpret_rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  interpret_rtl_or(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  
  rtl_li(&t1, 0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  rtl_update_ZFSF(&t0, id_dest->width);
  
  print_asm_template2(or);
}

make_EHelper(rol) {
  rtl_rol(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);

  rtl_msb(&t1, &t0, id_dest->width);
  rtl_get_CF(&t2);
  interpret_rtl_xor(&t1, &t1, &t2);
  rtl_set_CF(&t1);

  rtl_msb(&t1, &id_dest->val, id_dest->width);

  rtl_li(&t0, 1);
  rtl_setrelop(RELOP_EQ, &t0, &id_src->val, &t0);
  rtl_setrelop(RELOP_NE, &t1, &t2, &t1);
  if (t0) rtl_set_OF(&t1);

  print_asm_template2(rol);
}

make_EHelper(sar) {
  interpret_rtl_sar(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  interpret_rtl_shl(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  interpret_rtl_shr(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  rtl_update_ZFSF(&t0, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  t0 = ~id_dest->val;
  operand_write(id_dest, &t0);

  print_asm_template1(not);
}
