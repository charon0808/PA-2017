#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);


/* In PA2-1 */
make_EHelper(call);    /*modified*/
make_EHelper(push);
make_EHelper(pop);
make_EHelper(xor);
make_EHelper(sub);
make_EHelper(ret);


/* In PA2-2 */
make_EHelper(lea);
make_EHelper(and);
make_EHelper(dec);
make_EHelper(xchg);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(sbb);
make_EHelper(adc);
make_EHelper(or);
make_EHelper(set);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(not);
make_EHelper(inc);
make_EHelper(jmp);
make_EHelper(imul2);
make_EHelper(cltd);
make_EHelper(idiv);
make_EHelper(movsx);
make_EHelper(leave);
make_EHelper(div);
make_EHelper(imul1);
make_EHelper(shr);

/* In PA2-3 */
make_EHelper(in);
make_EHelper(out);
make_EHelper(mul);
make_EHelper(neg);
make_EHelper(rol);

/* In PA3-1 */
make_EHelper(lidt);
make_EHelper(int);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(iret);

/* In PA3-3 */
make_EHelper(cbw);

/* In PA4-1 */
make_EHelper(mov_cr2r);
make_EHelper(mov_r2cr);
