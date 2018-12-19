#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(decoding.jmp_eip);

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t0, cc);
  rtl_li(&t1, 0);
  rtl_jrelop(RELOP_NE, &t0, &t1, decoding.jmp_eip);

  print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  rtl_jr(&id_dest->val);

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(eip);
  // printf("%x\n", *eip + id_dest->imm);
  rtl_j(decoding.jmp_eip);

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  // only implement c3
  printf("eax: 0x%x, ecx: 0x%x, edx: 0x%x, ebx: 0x%x, esp: 0x%x, ebp: 0x%x, esi: 0x%x, edi: 0x%X, eflags: 0x%x\n", 
    reg_l(R_EAX), reg_l(R_ECX), reg_l(R_EDX), reg_l(R_EBX), reg_l(R_ESP), reg_l(R_EBP), reg_l(R_ESI), reg_l(R_EDI), cpu.eflags);
  
  rtl_pop(eip);
  // printf("%x\n", *eip);

  printf("eax: 0x%x, ecx: 0x%x, edx: 0x%x, ebx: 0x%x, esp: 0x%x, ebp: 0x%x, esi: 0x%x, edi: 0x%X, eflags: 0x%x\n", 
    reg_l(R_EAX), reg_l(R_ECX), reg_l(R_EDX), reg_l(R_EBX), reg_l(R_ESP), reg_l(R_EBP), reg_l(R_ESI), reg_l(R_EDI), cpu.eflags);
  
  print_asm("ret");
}

make_EHelper(call_rm) {
  TODO();

  print_asm("call *%s", id_dest->str);
}
