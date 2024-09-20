#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
void
_main()
{
  extern int main();
  main();
  exit(0);
}

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
    cdst[i] = c;
  }
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

/*
* read the next character from a string and returns 
* the amount of characters read
*/
int
fgets(char *buf, int size, int fd)
{
  int i, cc;
  char c;

  for(i=0; i+1 < size; ){
    cc = read(fd, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return i;
}

char*
gets(char *buf, int max){
	fgets(buf, max, 0);
	return buf;
}

/*
* reads an entire line from the file discripter, storing the address of the buffer
* containting the text into *lineptr. The buffer is null-terminated and
* includes the newline character, if one was found.
*/
int getline(char **lineptr, uint *n, int fd){
	//checks if the pointer to lineptr is not intialized
	//initiazes it if it isn't initialized  and gives a set buffer size to start out
	if(*n == 0 || *lineptr == 0){
		*n = 128;
		*lineptr = malloc(*n);
	}
	
	//total keeps track of the total amount of characters read so far
	//and helps with keeping track of what index the char string is at
	char *buf = *lineptr;
	int total = 0;
	
	while(true){
		//reads the amount of characters in an individual line
		int cc = fgets(buf + total, *n - total, fd);

		//returns total if it's at the end of file
		if(cc == 0){
			return total;
		}
		//return -1 since there was an error when running fgets
		if(cc == -1){
			return -1;
		}

		//no error so the amount of characters is added to the running total
		//so far
		total += cc;
			
		//checks if there is a new line at the end of file
		if(buf[total - 1] == '\n' || buf[total - 1] == '\r'){
			break;
		}
		//performs calculation for new buffer size
		uint new_n = *n * 2;
		//creates a new string and initializes base sise
		char *new_buf = malloc(new_n);
		//copys contents of buf into new_buf
		memcpy(new_buf, buf, *n);
		//frees the space used by malloc
		free(buf);
		//buff is set to new_buf that has extra space for future traversals
		buf = new_buf;

		//sets new max size
		*n = new_n;
		//assigns lineptr equal to buf
		*lineptr = buf;
	}

	return total;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
    src += n;
    while(n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}

int
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  return memmove(dst, src, n);
}
