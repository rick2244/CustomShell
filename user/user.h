#define NULL ((void *) 0)

#define bool _Bool
#define true 1
#define false 0

#define SEEK_SET        0
#define STDIN_FILENO    0
#define SEEK_CUR        1
#define STDOUT_FILENO   1
#define SEEK_END        2
#define STDERR_FILENO   2
#define FSM_FIRST_FIT   0
#define FSM_BEST_FIT    1
#define FSM_WORST_FIT   2

struct stat;

// system calls
int fork(void);
int exit(int return_status) __attribute__((noreturn));
int wait(int *status);
int wait2(int *exist_status, int *syscall_cnt);
int pipe(int *pipefd);
int write(int fd, const void* buf , int count);
int read(int fd, void* buf, int count);
int close(int fd);
int kill(int pid);
//question about char**
int exec(const char* /*str*/ pathname, char** argv);

int open(const char* /*str*/ pathname, int flags);
int mknod(const char* /*str*/ pathname , short mode, short dev);
int unlink(const char* /*str*/ pathname);
int fstat(int fd, struct stat* statbuf);
int link(const char* /*str*/ oldpath, const char* /*str*/ newpath);
int mkdir(const char* /*str*/ pathname);
int chdir(const char* /*str*/ pathname);
int dup(int oldfd);
int getpid(void);
char* sbrk(int increment);
int sleep(int seconds);
int uptime(void);
int shut(void);
int reboot(void);
int utime(void);
int strace_on(void);
int getcwd(char *, int);

// ulib.c
int stat(const char*, struct stat*);
char* strcpy(char*, const char*);
void *memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void fprintf(int, const char*, ...);
void printf(const char*, ...);
int getline(char**, uint*, int);
int fgets(char*, int, int);
char* gets(char*, int);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint size);
void* realloc(void*, uint64);
void malloc_print();
bool malloc_leaks();
void malloc_name(void*, const char*);
void malloc_setfsm(uint);
void malloc_scribble();
void free(void*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
void *memcpy(void *, const void *, uint);