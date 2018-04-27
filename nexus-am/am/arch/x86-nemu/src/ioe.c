#include <am.h>
#include <x86.h>

#include "stdio.h"

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
    boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
    unsigned long time=inl (RTC_PORT)-boot_time;
    //printf("In ioe.c, _uptime, time=%d\n",time);
    return time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
    .width  = 400,
    .height = 300,
};

extern void* memcpy(void *, const void *, int);

static inline int min(int x, int y) {
  return (x < y) ? x : y;
}


#define W 400
#define H 300

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
    int cp_bytes = sizeof(uint32_t) * min(w, _screen.width - x);
    for (int j = 0; j < h && y + j < _screen.height; j ++) {
        memcpy(&fb[(y + j) * W + x], pixels, cp_bytes);
        pixels += w;
    }
}


void _draw_sync() {
}

int _read_key() {
    if (inb(0x64)==1){
        return inl(0x60);
    }
    return _KEY_NONE;
}
