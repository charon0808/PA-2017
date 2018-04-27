#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_none,   //0
  SYS_open,   //1
  SYS_read,   //2 
  SYS_write,  //3
  SYS_exit,   //4
  SYS_kill,   //5 
  SYS_getpid, //6
  SYS_close,  //7
  SYS_lseek,  //8
  SYS_brk,    //9
  SYS_fstat,  //10
  SYS_time,   //11
  SYS_signal, //12
  SYS_execve, //13
  SYS_fork,   //14
  SYS_link,   //15
  SYS_unlink, //16
  SYS_wait,   //17
  SYS_times,  //18
  SYS_gettimeofday //19
};

#endif
