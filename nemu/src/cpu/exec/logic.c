#include "cpu/exec.h"

make_EHelper(test) {
    //TODO();
    if ((decoding.opcode&0xff)==0x84||(decoding.opcode&0xff)==0xf6){
        id_dest->val&=0x000000ff;
        id_src->val&=0x000000ff;
        id_dest->width=1;
    }
    else if ((decoding.opcode&0xff)==0xa8){
        id_dest->width=id_src->width=1;
        id_dest->val=cpu.eax&0x000000ff;
    }
    else if ((decoding.opcode&0xff)==0xa9){
        id_dest->width=id_src->width=2;
        id_dest->val=cpu.eax&0x0000ffff;
    }
    else {
        id_dest->width=id_src->width=decoding.is_operand_size_16?2:4;
    }
    //printf ("In exec_test, id_dest->val=0x%x, id_src->val=0x%x",id_dest->val,id_src->val);
    id_dest->val&=id_src->val;
    cpu.EFLAGS.OF=cpu.EFLAGS.CF=0;
    rtl_update_ZFSF(&id_dest->val,id_dest->width);
    print_asm_template2(test);
}

make_EHelper(and) {      /* modified */
    //TODO();
    id_dest->width=id_src->width=decoding.is_operand_size_16?2:4;
    if ((decoding.opcode&0xff)==0x22)
        id_dest->width=1;
    else if ((decoding.opcode&0xff)==0x83) {
        id_src->width=1;
        rtl_sext(&id_src->val,&id_src->val,id_src->width);
    }
    else if ((decoding.opcode&0xff)==0x25){
        id_dest->type=OP_TYPE_REG;
        id_dest->val=cpu.eax;
        id_dest->reg=0;
    }
    else if ((decoding.opcode&0xff)==0x20){
        id_dest->width=id_src->width=1;
    }
    else if ((decoding.opcode&0xff)==0x80){
        id_dest->width=id_src->width=1;
    }
    id_dest->val&=id_src->val;
    cpu.EFLAGS.OF=cpu.EFLAGS.CF=0;
    rtl_update_ZFSF(&id_dest->val,id_dest->width);
    //rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    operand_write(id_dest,&id_dest->val);
    print_asm_template2(and);
}

make_EHelper(xor) {
    //TODO();
    uint16_t hah=decoding.opcode&0xff;
    id_dest->width=id_src->width=decoding.is_operand_size_16?2:4;
    if (hah==0x30||hah==0x32||hah==0x34||hah==0x80){
        id_dest->width=id_src->width=1;
    }
    decoding.dest.val=decoding.src.val^decoding.dest.val;
    //rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    operand_write(id_dest,&id_dest->val);
    cpu.EFLAGS.OF=cpu.EFLAGS.CF=0;
    rtl_update_ZFSF(&decoding.dest.val,decoding.dest.width);
    print_asm_template2(xor);
}

make_EHelper(or) {
    //TODO();
    if (decoding.is_operand_size_16){
        id_dest->width=id_src->width=2;
    }
    else id_dest->width=id_src->width=4;
    if ((decoding.opcode&0xff)==0x0a||(decoding.opcode&0xff)==0x80||(decoding.opcode&0xff)==0x08){
        id_dest->width=id_src->width=1;
    }
    id_dest->val|=id_src->val;
    operand_write(id_dest,&id_dest->val);
    cpu.EFLAGS.OF=cpu.EFLAGS.CF=0;
    rtl_update_ZFSF(&id_dest->val,id_dest->width);
    print_asm_template2(or);
}

make_EHelper(sar) {
    //TODO();
    // unnecessary to update CF and OF in NEMU
    rtlreg_t  my_sign;
    rtl_msb(&my_sign,&id_dest->val,id_dest->width);
    id_dest->val=(id_dest->val)>>(id_src->val);
    if (my_sign){
        id_dest->val|=(0xffffffff<<(32-id_src->val));
    }
    rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    rtl_update_ZFSF(&id_dest->val,id_dest->width);
    print_asm_template2(sar);
}

make_EHelper(shl) {
    //TODO();
    // unnecessary to update CF and OF in NEMU
    id_dest->val=(id_dest->val)<<(id_src->val);
    rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    rtl_update_ZFSF(&id_dest->val,id_dest->width);
    print_asm_template2(shl);
}

make_EHelper(shr) {
    //TODO();
    // unnecessary to update CF and OF in NEMU
    id_dest->val=(id_dest->val)>>(id_src->val);
    rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    rtl_update_ZFSF(&id_dest->val,id_dest->width);
    print_asm_template2(shr);
}

make_EHelper(setcc) {
    uint8_t subcode = decoding.opcode & 0xf;
    rtl_setcc(&t2, subcode);
    operand_write(id_dest, &t2);

    print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
    //TODO();
    if ((decoding.opcode&0xff)==0xf6){
        id_dest->width=1;
    }
    id_dest->val=~id_dest->val;
    rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    print_asm_template1(not);
}
