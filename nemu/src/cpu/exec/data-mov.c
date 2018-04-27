#include "cpu/exec.h"

make_EHelper(mov) {
    operand_write(id_dest, &id_src->val);
    print_asm_template2(mov);
}

make_EHelper(push) {
    //TODO();
    rtl_push(&id_dest->val);
    print_asm_template1(push);
}

make_EHelper(pop) {
    //TODO();
    rtl_pop (&decoding.dest.val);
    rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    print_asm_template1(pop);
}

make_EHelper(pusha) {
    //TODO();
    if (!decoding.is_operand_size_16){
        uint32_t tmp=cpu.esp;
        rtl_push(&cpu.eax);
        rtl_push(&cpu.ecx);
        rtl_push(&cpu.edx);
        rtl_push(&cpu.ebx);
        rtl_push(&tmp);
        rtl_push(&cpu.ebp);
        rtl_push(&cpu.esi);
        rtl_push(&cpu.edi);
    }

    else {
        uint32_t ah[8];
        for(int i=0;i<8;i++){
            ah[i]=cpu.gpr[i]._16|0x0000ffff;
        }
        uint32_t tmp=cpu.esp&0xffff;
        rtl_push(ah);
        rtl_push(ah+1);
        rtl_push(ah+2);
        rtl_push(ah+3);
        rtl_push(&tmp);
        rtl_push(ah+5);
        rtl_push(ah+6);
        rtl_push(ah+7);
    }

    print_asm("pusha");
}

make_EHelper(popa) {
    //TODO();
    uint32_t throwaway;
    rtl_pop(&cpu.edi);
    rtl_pop(&cpu.esi);
    rtl_pop(&cpu.ebp);
    rtl_pop(&throwaway);
    rtl_pop(&cpu.ebx);
    rtl_pop(&cpu.edx);
    rtl_pop(&cpu.ecx);
    rtl_pop(&cpu.eax);
    print_asm("popa");
}

make_EHelper(leave) {   //modified
    //TODO();
    if (decoding.is_operand_size_16){
        cpu.gpr[4]._16=cpu.gpr[5]._16;
    }
    else {
        cpu.gpr[4]._32=cpu.gpr[5]._32;
    }
    if (decoding.is_operand_size_16){
        uint32_t ah;
        rtl_pop(&ah);
        cpu.gpr[5]._16=ah&0x0000ffff;
    }
    else rtl_pop(&cpu.gpr[5]._32);
    print_asm("leave");
}

make_EHelper(cltd) {
    /* if (decoding.is_operand_size_16) {
    //TODO();
    id_src->val=cpu.gpr[0]._16;
    id_dest->val=id_src->val<0?0xffff:0x0;
    id_dest->width=2;
    }
    else {
    //TODO();
    id_src->val=cpu.gpr[0]._32;
    id_dest->val=id_src->val<0?0xffffffff:0x0;
    id_dest->width=4;
    }*/
    if (decoding.is_operand_size_16){
        id_dest->width=id_src->width=2;
        if ((cpu.eax&0x00008000)==0x00008000){
            id_dest->val=0x0000ffff;
        }
        else id_dest->val=0;
    }
    else {
        id_dest->width=id_src->width=4;
        if ((cpu.eax&0x80000000)==0x80000000){
            id_dest->val=0xffffffff;
        }
        else id_dest->val=0;
    }

    rtl_sr(2,id_dest->width,&id_dest->val);
    print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
    if (decoding.is_operand_size_16) {
        TODO();
    }
    else {
        TODO();
    }

    print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(cbw)
{
    id_dest->val=cpu.eax;
    if (decoding.is_operand_size_16){
        if ((id_dest->val&0x80)==0x80){
            id_dest->val&=0x0000ffff;
        }
        else id_dest->val&=0x000000ff;
        rtl_sr(0,2,&id_dest->val);
    }
    else {
        if ((id_dest->val&0x8000)==0x8000){
            id_dest->val&=0xffffffff;
        }
        else id_dest->val&=0x0000ffff;
        rtl_sr(0,4,&id_dest->val);
    }
    print_asm(decoding.is_operand_size_16? "cbw" : "cwde");
}

make_EHelper(movsx) {
    id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
    if ((decoding.opcode&0xff)==0xbe){
        id_src->width=1;
    }
    else {
        id_src->width=2;
    }

    rtl_sext(&t2, &id_src->val, id_src->width);
    operand_write(id_dest, &t2);
    print_asm_template2(movsx);
}

make_EHelper(movzx) {
    id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
    if ((decoding.opcode&0xff)==0xb6){
        id_src->width=1;
        id_src->val&=0x000000ff;
    }
    else {
        id_src->width=2;
        id_src->val&=0x0000ffff;
    }
    //rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    operand_write(id_dest, &id_src->val);
    print_asm_template2(movzx);
}

make_EHelper(lea) {
    rtl_li(&t2, id_src->addr);
    operand_write(id_dest, &t2);
    print_asm_template2(lea);
}

make_EHelper(xchg)    /*modified*/
{
    id_dest->width=decoding.is_operand_size_16?2:4;
    if (decoding.is_operand_size_16){
        uint16_t temp=id_dest->val;
        id_dest->val=cpu.gpr[0]._16;
        cpu.gpr[0]._16=temp;
    }
    else {
        uint32_t temp=id_dest->val;
        id_dest->val=cpu.gpr[0]._32;
        cpu.gpr[0]._32=temp;
        //rtl_sr(id_src->reg,id_src->width,&id_src->val);
    }
    rtl_sr(id_dest->reg,id_dest->width,&id_dest->val);
    //printf ("%d\n",id_src->reg);
    strcpy(id_src->str,decoding.is_operand_size_16?"%ax":"%eax");
    print_asm_template2(xchg);
}

make_EHelper(rol)
{
    uint32_t temp=id_src->val&0xff;
    while (temp){
        uint32_t tmpcf;
        rtl_msb(&tmpcf,&id_dest->val,id_dest->width);
        id_dest->val=id_dest->val*2+tmpcf;
        temp--;
    }
    if (id_src->val==1){
        uint32_t tmpcf;
        rtl_msb(&tmpcf,&id_dest->val,id_dest->width);
        if (tmpcf!=cpu.EFLAGS.CF){
            cpu.EFLAGS.OF=1;
        }
        else cpu.EFLAGS.OF=0;
    }
}
