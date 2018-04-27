#include "cpu/rtl.h"
#include "cpu/exec.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,    // 0  1  2  3  
    CC_E, CC_NE, CC_BE, CC_NBE,   // 4  5  6  7
    CC_S, CC_NS, CC_P,  CC_NP,    // 8  9  10  11
    CC_L, CC_NL, CC_LE, CC_NLE    // 12  13  14  15
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:*dest=cpu.EFLAGS.OF?0x1:0x0; break;
    case CC_B:*dest=cpu.EFLAGS.CF?0x1:0x0; break;
    case CC_E:*dest=cpu.EFLAGS.ZF?0x1:0x0; break;
    case CC_BE:*dest=cpu.EFLAGS.CF||cpu.EFLAGS.ZF?0x1:0x0; break;
    case CC_S:*dest=cpu.EFLAGS.SF?0x1:0x0; break;
    case CC_L:*dest=cpu.EFLAGS.SF!=cpu.EFLAGS.OF?0x1:0x0; break;
    case CC_LE:*dest=cpu.EFLAGS.ZF||(cpu.EFLAGS.SF!=cpu.EFLAGS.OF)?0x1:0x0; break;
    //TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}

make_EHelper(set){
    uint8_t subcode=decoding.opcode&0xf;
    rtl_setcc(&id_dest->val,subcode);
    id_dest->width=1;
    //printf("In exec_set, id_dest->reg=0x%x\n",id_dest->reg);
    operand_write(id_dest,&id_dest->val);
    //rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    char my_output[16][3]={"o","no","b","nb","e","ne","be","nbe","s","ns","p","np","l","nl","le","nle"};
    strcpy(my_output[0],"o");
    print_asm("set%s",my_output[subcode&0xe]);
}
