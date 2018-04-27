#include "fs.h"

typedef struct {
    char *name;
    size_t size;
    off_t disk_offset;
    off_t open_offset;   //added
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    {"stdin (note that this is not the actual stdin)", 0, 0},
    {"stdout (note that this is not the actual stdout)", 0, 0},
    {"stderr (note that this is not the actual stderr)", 0, 0},
    [FD_FB] = {"/dev/fb", 0, 0},
    [FD_EVENTS] = {"/dev/events", 0, 0},
    [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
    file_table[FD_FB].size=400*300*4;
    // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname,int flags,int mode)
{
    for (int i=0;i<=sizeof(file_table)/sizeof(Finfo);i++){
        if (strcmp(pathname,file_table[i].name)==0){
            file_table[i].open_offset=0;
            return i;
        }
    }
    //assert(0);  //No such pathname file be found
    return -1;
}

extern int ramdisk_read(void*,off_t,size_t);
extern int ramdisk_write(const void*,off_t,size_t);

off_t fs_lseek(int,off_t,int);

extern void dispinfo_read(void*,off_t,size_t);
extern size_t events_read(void*,size_t);


ssize_t fs_read(int fd, void *buf, size_t len)
{
    if (file_table[fd].open_offset+len>file_table[fd].size){
        len=file_table[fd].size-file_table[fd].open_offset;
        if (len<0){
            len=0;
        }
    }
    if (fd==5){    // dispinfo_read 
        dispinfo_read(buf,fs_lseek(fd,0,SEEK_CUR),len);
    }
    else if (fd==4){  //events_read
        len=events_read(buf,len);
    }
    else ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
    fs_lseek(fd,len,SEEK_CUR);
    return len;
}

extern void fb_write(const void *,off_t,size_t); 
ssize_t fs_write(int fd, const void *buf, size_t len)
{
    //if ((int)len>(int)fs_filesz(fd))
    //    assert(0);
    //printf("In fs_write, fd=%d\n",fd);
    if (file_table[fd].open_offset+len>file_table[fd].size){
        len=file_table[fd].size-file_table[fd].open_offset;
        if (len<0){
            len=0;
        }
        //fs_lseek(fd,0,SEEK_END);
    }
    if (fd==3){
        fb_write(buf,fs_lseek(fd,0,SEEK_CUR),len);
    }
    else ramdisk_write((void*)buf,(off_t)file_table[fd].disk_offset+file_table[fd].open_offset,len);
    fs_lseek(fd,len,SEEK_CUR);
    return len;
}

off_t fs_lseek(int fd, off_t offset, int whence)
{
    switch(whence){
        /*0*/ case SEEK_SET:file_table[fd].open_offset=offset;break;
        /*1*/ case SEEK_CUR:file_table[fd].open_offset+=offset;break;
        /*2*/ case SEEK_END:file_table[fd].open_offset=file_table[fd].size+offset;break;
    }

    //printf  ("In fs_lseek,fd=%d, open_offset=%d\n",fd,file_table[fd].open_offset);
    return file_table[fd].open_offset;
}

int fs_close(int fd)
{
    return 0;
}

size_t fs_filesz(int fd)
{
    return file_table[fd].size;
}
