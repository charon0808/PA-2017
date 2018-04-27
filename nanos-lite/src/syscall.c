#include "common.h"
#include "syscall.h"
#include "arch.h"

uintptr_t sys_none()
{
    return 1;
}

uintptr_t sys_exit(uintptr_t ah)
{
    //printf("In sys_exit, exit code=%d\n",ah);
    _halt(ah);
    return 1;
}

extern size_t fs_write(int,const void*,size_t);
extern void fb_write(const void *,off_t,size_t); 
extern int fs_open(const char *,int,int);
extern size_t fs_read(int,void*,size_t);
extern off_t fs_lseek(int,off_t,int);
extern int fs_close(int);


uintptr_t sys_write(_RegSet *r)
{

    int fd=SYSCALL_ARG2(r);
    char* buf=(char*)SYSCALL_ARG3(r);
    size_t count=SYSCALL_ARG4(r);
    if (fd==1||fd==2){
        int i;
        for (i=0;i<count;i++){
            _putc(buf[i]);
        }
        //printf("%s",buf);
        return count;
    }
    else{
        return fs_write(fd,(void*)buf,count);
    }
}

int mm_brk(uint32_t);

uintptr_t sys_brk(_RegSet* r)
{
    uintptr_t pro_brk=SYSCALL_ARG2(r);
    _heap.end=(void*)pro_brk;
    return mm_brk(pro_brk);
}


uintptr_t sys_open(_RegSet* r)
{
    //printf("In sys_open\n");
    return fs_open((char*)SYSCALL_ARG2(r),(int)SYSCALL_ARG3(r),(int)SYSCALL_ARG4(r)); 
}


uintptr_t sys_read(_RegSet* r)
{
    //int fd=SYSCALL_ARG2(r);
    //printf("In sys_read, fd=%d\n",fd);
   return fs_read((int)SYSCALL_ARG2(r),(void*)SYSCALL_ARG3(r),(size_t)SYSCALL_ARG4(r));

}

uintptr_t sys_lseek(_RegSet* r)
{
    //printf("In sys_lseek,offset=%d\n",SYSCALL_ARG3(r));
    return fs_lseek((int)SYSCALL_ARG2(r),(off_t)SYSCALL_ARG3(r),(int)SYSCALL_ARG4(r));

}

uintptr_t sys_close(_RegSet *r)
{
    // printf("In sys_clean\n");
    return fs_close((int)SYSCALL_ARG2(r));

}

_RegSet* do_syscall(_RegSet *r) {
    uintptr_t a[4];
    a[0] = SYSCALL_ARG1(r);
    //printf("In do_syscall, a[0]=%d\n",a[0]);
    switch (a[0]) {
        case SYS_none:  SYSCALL_ARG1(r)= sys_none();                break;
        case SYS_exit:  SYSCALL_ARG1(r)= sys_exit(SYSCALL_ARG2(r)); break;
        case SYS_write: SYSCALL_ARG1(r)= sys_write(r);              break;
        case SYS_brk:   SYSCALL_ARG1(r)= sys_brk(r);                break;
        case SYS_open:  SYSCALL_ARG1(r)= sys_open(r);               break;
        case SYS_read:  SYSCALL_ARG1(r)= sys_read(r);               break;
        case SYS_lseek: SYSCALL_ARG1(r)= sys_lseek(r);              break;
        case SYS_close: SYSCALL_ARG1(r)= sys_close(r);              break;
        default: panic("Unhandled syscall ID = %d", a[0]);
    }
    return r;
}

