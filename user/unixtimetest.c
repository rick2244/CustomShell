#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(void){
	uint64 time = utime();
	printf("Time in seconds %d\n", time);
	return 0;
}
