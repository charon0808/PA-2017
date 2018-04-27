#include "cpu/exec.h"
#include "memory/mmu.h"

extern GateDesc idt[256];



void raise_intr(uint8_t NO, vaddr_t ret_addr) {
    /* TODO: Trigger an interrupt/exception with ``NO''.
     * That is, use ``NO'' to index the IDT.
     */

    //printf  ("In raise_intr, NO=0x%x, cpu.EFLAGS.val=0x%x, ret_addr=0x%x\n",NO,cpu.EFLAGS.val,ret_addr);

    rtl_push(&cpu.EFLAGS.val);
    if (NO==32)
        cpu.EFLAGS.IF=0;       // set IF
    rtl_push(&cpu.cs);
    rtl_push(&ret_addr);


    uint32_t idt_addr=cpu.idtr.base+NO*8;
    uint32_t lx_high=vaddr_read(idt_addr+4,4);
    uint16_t lx_low=vaddr_read(idt_addr,2);
    decoding.is_jmp=1;
    decoding.jmp_eip=(lx_high&0xffff0000)|(lx_low&0xffff);
    //TODO();
}

void dev_raise_intr() {
    cpu.INTR=1;     // set INTR
}
