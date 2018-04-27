#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {  //modified
    //TODO();
    rtl_lm(&id_src->val,&id_dest->addr,2);
    id_dest->addr+=2;
    rtl_lm(&id_dest->val,&id_dest->addr,4);
    if (decoding.is_operand_size_16){
        cpu.idtr.limit=id_src->val&0x0000ffff;
        cpu.idtr.base=id_dest->val&0x00ffffff;
    }
    else {
        cpu.idtr.limit=id_src->val&0x0000ffff;
        cpu.idtr.base=id_dest->val;
    }
    print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
    //TODO();
    id_dest->val=id_src->val;
    if (id_dest->reg==0)
        cpu.cr0=id_dest->val;
    else if (id_dest->reg==3)
        cpu.cr3=id_dest->val;
    print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
    //TODO();
    
    id_dest->val=id_src->val;
    operand_write(id_dest,&id_dest->val);

    print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
    diff_test_skip_qemu();
#endif
}

extern void raise_intr(uint8_t NO,vaddr_t ret_addr);

make_EHelper(int) {
    //TODO();
    uint8_t NO=id_dest->val&0xff;
    //raise_intr(NO,(NO==0x80)?cpu.eip+2:cpu.eip);
    raise_intr(NO,decoding.seq_eip);
    print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
    diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
    //TODO();
    //printf  ("before iret, cpu.EFLAGS.IF=%d\n",cpu.EFLAGS.IF);
    rtl_pop(&decoding.jmp_eip);
    rtl_pop(&cpu.cs);
    rtl_pop(&cpu.EFLAGS.val);
    decoding.is_jmp=1;
    //printf ("In iret, decoding.jmp_eip=0x%x, cpu.EFLAGS.IF=%d\n",decoding.jmp_eip,cpu.EFLAGS.IF);
    //decoding.seq_eip=decoding.jmp_eip;
    print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
    //TODO();
    uint32_t hah=pio_read(id_src->val,id_src->width);
    operand_write(id_dest,&hah);
    print_asm_template2(in);

#ifdef DIFF_TEST
    diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
    //TODO();
    pio_write(id_dest->val,id_src->width,id_src->val);

    print_asm_template2(out);

#ifdef DIFF_TEST
    diff_test_skip_qemu();
#endif
}
