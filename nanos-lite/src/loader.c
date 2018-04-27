#include "common.h"

//#define DEFAULT_ENTRY ((void *)0x4000000)
#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);

extern size_t get_ramdisk_size();


extern int fs_open(const char *,int,int);
extern ssize_t fs_read(int,void*,size_t);
extern ssize_t fs_close(int);
extern size_t fs_filesz(int);
extern void* new_page(void);


uintptr_t loader(_Protect *as, const char *filename) {    //modified
    //TODO();
    uint32_t fd=fs_open(filename,0,0);
    uint32_t len=fs_filesz(fd);
    uint32_t page_number=(len+4095)/4096;
    //Log("In loader, filename=%s, len=0x%x, page_number=%d, DEFAULT_ENTRY=0x%x\n",filename,len,page_number,DEFAULT_ENTRY);
    for (int i=0;i<page_number;i++){
        void* new_addr=new_page();                // a new page
        _map(as,DEFAULT_ENTRY+i*4096,new_addr);   // map the new page
        fs_read(fd,new_addr,4096);                // read a page to the new page
    }

    //fs_read(fd,DEFAULT_ENTRY,len);
    fs_close(fd);
    return (uintptr_t)DEFAULT_ENTRY;
}
