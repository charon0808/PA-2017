#include "common.h"


#define NAME(key) \
    [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [_KEY_NONE] = "NONE",
    _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
    int my_key=_read_key();
    //printf("In events_read, key=0x%x, time=%d\n",my_key,_uptime());
    if (my_key!=_KEY_NONE){
        sprintf(buf,"%s %s\n",((my_key&0x8000)?"kd":"ku"),keyname[my_key&0x7fff]);
    }
    else {
        sprintf(buf,"t %d\n",_uptime());
    }
    return strlen(buf);
}

extern off_t fs_lseek(int,off_t,int); 
extern ssize_t fs_read(int,void*,size_t);
extern ssize_t fs_write(int,const void*,size_t);
static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
    //printf("In dispinfo_read\n");
    offset/=sizeof(char),len/=sizeof(char);
    int i;
    for (i=0;i<len;i++){
        ((char*)buf)[i]=dispinfo[i+offset];
    }
}


void fb_write(const void *buf, off_t offset, size_t len) {
    //printf("In fb_write\n");
    uint32_t x=(offset/4)%_screen.width;
    uint32_t y=(offset/4)/_screen.width;
    // printf("In fb_write, x=%d  y=%d, offset=%d\n",x,y,offset);
    _draw_rect(buf,x,y,len/4,1);
}

void init_device() {
    _ioe_init();
    strcpy(dispinfo,"WIDTH:400\nHEIGHT:300");
    // TODO: print the string to array `dispinfo` with the format
    // described in the Navy-apps convention
}
