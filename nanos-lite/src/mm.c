#include "proc.h"
#include "memory.h"

static void *pf = NULL;

void* new_page(void) {
    assert(pf < (void *)_heap.end);
    void *p = pf;
    pf += PGSIZE;
    return p;
}

void free_page(void *p) {
    panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {

    if (current->cur_brk==0){
        current->cur_brk=current->max_brk=new_brk;
    }
    else {
        if (new_brk>current->max_brk){
            if ((current->max_brk)&0xfff){
                current->max_brk&=0xfffff000;
                current->max_brk+=0x1000;
            }
            //if (new_brk<=current->max_brk){
            //    new_brk+=0x1000;
            //}
            uint32_t len=new_brk - current->max_brk;
            uint32_t page_number=(len+4095)/4096;
            //printf ("In mm_brk, new_brk=0x%x, current->max_brk=0x%x, len=0x%x, page_number=%d\n",new_brk,current->max_brk,len,page_number);
            for (int i=0;i<page_number;i++){
                void* new_addr=new_page();
                _map(&current->as,(void*)(current->max_brk+i*4096),new_addr);
            }
            current->max_brk=new_brk;
        }
        current->cur_brk=new_brk;
    }
    return 0;
}

void init_mm() {
    pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
    Log("free physical pages starting from %p", pf);

    _pte_init(new_page, free_page);
}
