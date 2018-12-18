#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// data-mov
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);

// control
make_EHelper(call);
make_EHelper(ret);


// arith
make_EHelper(sub);

// logic
make_EHelper(xor);
make_EHelper(and);
make_EHelper(or);

