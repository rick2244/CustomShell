#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(void){
	reboot();
	return 0;
}
