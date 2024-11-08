#include "defs.h"
#include "syscall.h"

static int
read_int(int n)
{
  int i;
  argint(n, &i);
  return i;
}


// Function to read a string from the syscall arguments
static char* read_str(int n) {
    static char buf[512]; // Temporary buffer to hold the string
    // Call argstr to populate the buffer
    argstr(n, buf, 512);
    return buf;
}












void
strace(struct proc *p, int syscall_num, int ret_val)
{
  
  // TODO given a system call number, figure out what tracing info to show.
  //      The code generation below should get you close, but you will need to
  //      modify this function and the code generation procedure.

//hint a giant switch case
// Adding a comment so the following generated code doesn't actually run:

    switch(syscall_num){
      case SYS_fork:
        printf("[%d|%s] fork() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_exit:
        printf("[%d|%s] exit(return_status = %d) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_wait:
        printf("[%d|%s] wait(status = %p) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_pipe:
        printf("[%d|%s] pipe(pipefd = %p) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_read:
        printf("[%d|%s] read(fd = %d, buf = %p, count = %d) = %d\n", p->pid, p->name, read_int(0), read_int(1), read_int(2),  ret_val);
        break;
      case SYS_write:
        printf("[%d|%s] write(fd = %d, buf = %p, count = %d) = %d\n", p->pid, p->name, read_int(0), read_int(1), read_int(2),  ret_val);
        break;
      case SYS_close:
        printf("[%d|%s] close(fd = %d) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_kill:
        printf("[%d|%s] kill(pid = %d) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_exec:
        printf("[%d|%s] exec(pathname = %s, argv = %p) = %d\n", p->pid, p->name, read_str(0), read_int(1),  ret_val);
        break;
      case SYS_open:
        printf("[%d|%s] open(pathname = %s, flags = %d) = %d\n", p->pid, p->name, read_str(0), read_int(1),  ret_val);
        break;
      case SYS_mknod:
        printf("[%d|%s] mknod(pathname = %s, mode = %d, dev = %d) = %d\n", p->pid, p->name, read_str(0), read_int(1), read_int(2),  ret_val);
        break;
      case SYS_unlink:
        printf("[%d|%s] unlink(pathname = %s) = %d\n", p->pid, p->name, read_str(0),  ret_val);
        break;
      case SYS_fstat:
        printf("[%d|%s] fstat(fd = %d, statbuf = %p) = %d\n", p->pid, p->name, read_int(0), read_int(1),  ret_val);
        break;
      case SYS_link:
        printf("[%d|%s] link(oldpath = %s, newpath = %s) = %d\n", p->pid, p->name, read_str(0), read_str(1),  ret_val);
        break;
      case SYS_mkdir:
        printf("[%d|%s] mkdir(pathname = %s) = %d\n", p->pid, p->name, read_str(0),  ret_val);
        break;
      case SYS_chdir:
        printf("[%d|%s] chdir(pathname = %s) = %d\n", p->pid, p->name, read_str(0),  ret_val);
        break;
      case SYS_dup:
        printf("[%d|%s] dup(oldfd = %d) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_getpid:
        printf("[%d|%s] getpid() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_sbrk:
        printf("[%d|%s] sbrk(increment = %d) = %p\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_sleep:
        printf("[%d|%s] sleep(seconds = %d) = %d\n", p->pid, p->name, read_int(0),  ret_val);
        break;
      case SYS_uptime:
        printf("[%d|%s] uptime() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_shut:
        printf("[%d|%s] shut() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_reboot:
        printf("[%d|%s] reboot() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_utime:
        printf("[%d|%s] utime() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_strace_on:
        printf("[%d|%s] strace_on() = %d\n", p->pid, p->name,  ret_val);
        break;
      case SYS_wait2:
        printf("[%d|%s] wait2(exist_status = %p, syscall_cnt = %p) = %d\n", p->pid, p->name, read_int(0), read_int(1),  ret_val);
        break;
      case SYS_getcwd:
        printf("[%d|%s] getcwd(char* = %p, int = %d) = %d\n", p->pid, p->name, read_int(0), read_int(1),  ret_val);
        break;
    }
}
