#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
	int pid;
	
	pid = fork();
	if(pid == 0){
		strace_on();
		exec(argv[1], &argv[1]);
	}else{
		wait(NULL);
	}
	return 0;
}
