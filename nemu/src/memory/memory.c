#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
        Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
        guest_to_host(addr); \
        })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
    int my_map=is_mmio(addr);
    if (my_map!=-1){
        return mmio_read(addr,len,my_map);
    }
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
    int my_map=is_mmio(addr);
    if (my_map!=-1){
        mmio_write(addr,len,data,my_map);
        return;
    }
    memcpy(guest_to_host(addr), &data, len);
}

paddr_t page_translate(vaddr_t,int);

uint32_t vaddr_read(vaddr_t addr, int len) {
    //printf ("In vaddr_read, cr0=0x%x, cr3=0x%x\n",cpu.cr0,cpu.cr3);
    if (cpu.cr0&0x80000000){    // check PG
        //if (data cross the page the boungary){
        //    handle it later
        //    assert(0);
        //}
        //else {
        paddr_t paddr=page_translate(addr,0);
        return paddr_read(paddr,len);
        //}
    }
    else return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
    if (cpu.cr0&0x80000000) {
        //similar with the read
        paddr_t paddr=page_translate(addr,1);
        paddr_write(paddr,len,data);
    }
    else paddr_write(addr, len, data);
}

paddr_t page_translate(vaddr_t addr,int is_write)
{
    uint32_t offset=addr&0x00000fff;
    uint32_t page=(addr&0x003ff000)>>12;
    uint32_t dir=(addr&0xffc00000)>>22;
  
    CR3 xCR3;
    PDE xPDE;
    PTE xPTE;

    xCR3.val=cpu.cr3;

    uint32_t PDE_address=(xCR3.val&0xfffff000)+dir*4;
    xPDE.val=paddr_read(PDE_address,4);
    if (!xPDE.present){
        printf ("va=0x%x\n",addr);
    }
    assert(xPDE.present);
    
    uint32_t PTE_address=(xPDE.val&0xfffff000)+page*4;
    xPTE.val=paddr_read(PTE_address,4);
    if (!xPTE.present){
        printf("va=0x%x\n",addr);
    }
    assert(xPTE.present);

    xPDE.accessed=xPTE.accessed=1;      // set accessed
    if (is_write)                       // set dirty
        xPTE.dirty=1;
    paddr_write(PDE_address,4,xPDE.val);
    paddr_write(PTE_address,4,xPTE.val);
    
    uint32_t pa=(xPTE.val&0xfffff000)+offset;
    return pa;
}
