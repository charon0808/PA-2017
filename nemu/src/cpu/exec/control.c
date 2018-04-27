#include "cpu/exec.h"

make_EHelper(jmp) {
    // the target address is calculated at the decode stage
    decoding.is_jmp = 1;
    if ((decoding.opcode&0xff)==0xeb){
        rtl_sext(&id_src->val,&id_src->val,1);
        //id_dest->width=1;
        //id_dest->val&=0x000000ff;
        id_dest->val=id_src->val+decoding.seq_eip;
    }
    else if ((decoding.opcode&0xff)==0xe9){
        //rtl_sext(&id_src->val,&id_src->val,2);
        id_dest->val=id_dest->val+decoding.seq_eip;
    }
    if (decoding.is_operand_size_16){
        id_dest->val&=0x0000ffff;
    }
    decoding.jmp_eip=id_dest->val;
    print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
    // the target address is calculated at the decode stage
    uint8_t subcode = decoding.opcode & 0xf;
    rtl_setcc(&t2, subcode);
    decoding.is_jmp = t2;
    if (!(decoding.opcode&0x100)){
        rtl_sext(&id_src->val,&id_src->val,1);
        id_dest->val=decoding.seq_eip+id_src->val;
    }
    else id_dest->val=decoding.seq_eip+id_dest->val;
    if (decoding.is_operand_size_16)
        id_dest->val&=0x0000ffff;
    decoding.jmp_eip=id_dest->val;
    print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
    decoding.jmp_eip = id_dest->val;
    decoding.is_jmp = 1;

    print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {    /*modified*/
    // the target address is calculated at the decode stage
    //TODO();
    if ((decoding.opcode&0xff)==0xe8){
        decoding.jmp_eip=decoding.seq_eip+decoding.src.val;
    }
    else {
        decoding.jmp_eip=id_dest->val;
    }
    if (decoding.is_operand_size_16){
        decoding.jmp_eip&=0x0000ffff;
        decoding.seq_eip&=0x0000ffff;
    }
    rtl_push(&decoding.seq_eip);
    decoding.is_jmp=1;
    print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
    //TODO();
    decoding.is_jmp=1;
    rtl_pop(&decoding.jmp_eip);
    print_asm("ret");
}

make_EHelper(call_rm) {
    TODO();

    print_asm("call *%s", id_dest->str);
}
