#include "common.h"

extern _RegSet* do_syscall(_RegSet* r);
extern _RegSet* schedule(_RegSet* r);

static _RegSet* do_event(_Event e, _RegSet* r) {
    //printf("In do_events, r->eip=0x%x\n",r->eip);
    switch (e.event) {
        case _EVENT_SYSCALL:  r=do_syscall(r); break;
        case _EVENT_TRAP:     r=schedule(r);   break;
        case _EVENT_IRQ_TIME: r=schedule(r);   break; //Log("In _EVENT_IRQ_TIME");r=schedule(r);  break;
        default: panic("Unhandled event ID = %d", e.event);
    }
    return r;
}

void init_irq(void) {
    _asye_init(do_event);
}
