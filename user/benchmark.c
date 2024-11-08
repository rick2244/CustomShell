#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    int pid;

    pid = fork();
    if(pid == -1){
        fprintf(2, "Error forking");
    }
    int status = 1;
    uint64 start, end;
    int syscall_count = 1;
    start = utime();
    if(pid == 0){
        exec(argv[1], &argv[1]);
        exit(1);
    }else{
           // Call wait2 system call
	    printf("%p\n", &syscall_count);
        if (wait2(&status, &syscall_count) < 0) {
            return -1;
        }
        
        end = utime();
        uint64 elapsed = end - start;
        printf("---------------\n");
        printf("Benchmark Complete\n");
        printf("Time Elapsed: %d ms\n", elapsed);
        printf("System Calls: %d\n", syscall_count);
    }
    return 0;
}
