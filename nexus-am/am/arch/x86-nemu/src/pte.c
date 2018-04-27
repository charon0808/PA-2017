#include <x86.h>
#include <stdio.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
    {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
    palloc_f = palloc;
    pfree_f = pfree;

    int i;

    // make all PDEs invalid
    for (i = 0; i < NR_PDE; i ++) {
        kpdirs[i] = 0;
    }

    PTE *ptab = kptabs;
    for (i = 0; i < NR_KSEG_MAP; i ++) {
        uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
        uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
        for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
            // fill PDE
            kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

            // fill PTE
            PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
            PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
            for (; pte < pte_end; pte += PGSIZE) {
                *ptab = pte;
                ptab ++;
            }
        }
    }
    set_cr3(kpdirs);
    set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
    PDE *updir = (PDE*)(palloc_f());
    p->ptr = updir;
    // map kernel space
    for (int i = 0; i < NR_PDE; i ++) {
        updir[i] = kpdirs[i];
    }

    p->area.start = (void*)0x8000000;
    p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
    set_cr3(p->ptr);
}

extern void* new_page(void);

void _map(_Protect *p, void *va, void *pa) {
    uint32_t dir=(uint32_t)(((uint32_t)va&0xffc00000)>>22);
    uint32_t page=(uint32_t)(((uint32_t)va&0x003ff000)>>12);

    //printf ("In _map, va=0x%x, pa=0x%x\n",va,pa);
    PDE* PDE_ptr=(PDE*)(p->ptr);
    if (!(PDE_ptr[dir]&0x00000001)){
        void* new=new_page();               // a new page
        PDE_ptr[dir]|=((uint32_t)new&0xfffff000);     // set page frame
        PDE_ptr[dir]|=0x00000001;           // set present as 1   
    }

    PTE* PTE_ptr=(PTE*)(PDE_ptr[dir]&0xfffff000);
    PTE_ptr[page]|=((uint32_t)pa&0xfffff000);           // set page frame
    PTE_ptr[page]|=0x00000001;              // set present as 1

}

void _unmap(_Protect *p, void *va) {
}


_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {

    uint32_t* u_end=(uint32_t*)ustack.end;
    uint32_t* u_start=(uint32_t*)ustack.start;

    u_end[-1]=u_end[-2]=0;   // set stack frame of _start()
    
    u_end[-3]=0x00000202;   // set eflags
    u_end[-4]=8;   // cs
    u_end[-5]=(uint32_t)entry;  //eip
    u_end[-6]=0;   // error_code
    u_end[-7]=0; // irq
    for (int i=-8;i>=-15;i--){
        u_end[i]=0;
    }

    u_start[0]=(uint32_t)(u_end-15);
    return (_RegSet*)(u_end-15);
}
