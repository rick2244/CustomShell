#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define BUF_SIZE 1024

int
opfgets(char *buf, int size, int fd){
    //uses a static char to retain information that has already been read for future use
    static char read_buf[BUF_SIZE];
    //keeps track of read position for future use
    static int read_pos = 0;
    //keeds track of the length of the red
    static int read_len = 0;

    int i = 0;
    char c;

    //runs till i is fills up buf
    while(i + 1 < size){
        if(read_pos >= read_len){
            read_len = read(fd, &read_buf, BUF_SIZE);
            //checks if there is no more data to read
            //if so break out of loop
            if(read_len < 1) return 0; 
            //resets to 0 so that it starts copying from start
            read_pos = 0;
        }

        c = read_buf[read_pos++];

        buf[i++] = c;

        if(c == '\n') break;
    }
    //closes buf 
    buf[i] = '\0';
    return i;
}

int
main(int argc, char *argv[])
{
  if (argc <= 1) {
    fprintf(2, "Usage: %s filename\n", argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  //printf("%s", argv[1]);
  char buf[128];
  int line_count = 0;
  while (opfgets(buf, 128, fd) > 0 ) {
    printf("Line %d: %s", line_count++, buf);
  }

  return 0;
}