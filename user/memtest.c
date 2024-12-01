#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int 
main(void){
    char* a = malloc(1);
    char* b = malloc(512);
    char* c = malloc(905);

    free(a);
    free(b);
    free(c);

    malloc_print();

    return 0;
}