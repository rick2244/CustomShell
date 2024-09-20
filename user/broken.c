/**
 * @file broken.c
 * @author mmalensek
 *
 * This program contains a series of buggy, broken, or strange C functions for
 * you to ponder. Your job is to analyze each function, fix whatever bugs the
 * functions might have, and then explain what went wrong. Sometimes the
 * compiler will give you a hint.
 *
 *  ____________
 * < Good luck! >
 *  ------------
 *      \   ^__^
 *       \  (oo)\_______
 *          (__)\       )\/\
 *              ||----w |
 *              ||     ||
 */

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


static int func_counter = 1;
#define FUNC_START() printf("\n\n%d.) %s\n", func_counter++, __func__);


/**
 * This awesome code example was taken from the book 'Mastering C Pointers,'
 * one of the famously bad resources on learning C. It was trying to demonstrate
 * how to print 'BNGULAR'... with pointers...? Maybe?
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 *      Hint: where are string literals stored in memory?
 *      Hint: what is '66' in this example? Can we do better?
 *  The problem was that a string literal was being used, so I used a mutable data type
 *  Another thing I did was us the literal B instead of the ascii value correalated to it

 */
void
angular(void)
{
  FUNC_START();

  char a[] = "ANGULAR";
  a[0] = 'B';
  printf("%s\n", a);
}

/**
 * This function is the next step after 'Hello world' -- it takes user input and
 * prints it back out! (Wow).
 *
 * But, unfortunately, it doesn't work.
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 *
 *  The main problem was that name didn't have a defined size. So I gave the name vairable a defined size
 * 
 */
void
greeter(void)
{
  FUNC_START();

  char name[128];

  printf("Please enter your name: ");
  gets(name, 128);

  // Remove newline character
  char *p = name;
  int i = 0;
  while(p[i] != '\n'){
  	i+=1;
  }
  name[i] = '\0';


  printf("Hello, %s!\n", name);
}

//I rewrote linke 104 so that it is easier to read and chacnhed the max that i goes up to in line 107 to
//prevent overflow since c doesn't check itself for overflow.
/**
 * This 'useful' function prints out an array of integers with their indexes, or
 * at least tries to. It even has a special feature where it adds '12' to the
 * array.
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 *
 *  The simple fix was that the size max of the for loop repetitions exceeded the size of the array so I changed that to match the size of the array so that there would 
 *  not be overflow
 */
void
displayer(void)
{
  FUNC_START();
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Waggressive-loop-optimizations"

  int stuff[100] = { 0 };

  /* Can you guess what the following does without running the program? */
  /* Rewrite it so it's easier to read. */
  //14[stuff + 1] = 12;
  stuff[15] = 12;

  for (int i = 0; i < 100; ++i) {
    printf("%d: %d\n", i, stuff[i]);
  }

  #pragma GCC diagnostic pop
}

/**
 * Adds up the contents of an array and prints the total. Unfortunately the
 * total is wrong! See main() for the arguments that were passed in.
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 *
 *  The problem was that the size of the arr was incorrectly calculated using sizeof, which returns the amoutn of bytes of data not the actual length of the array
 *  I solved this by having a variable hold the size of the array in main passed in to the method to use as the max limiter in the for loop
 */
void
adder(int *arr, int len)
{
  FUNC_START();

  int total = 0;

  for (int i = 0; i < len; ++i) {
    total += arr[i];
  }

  printf("Total is: %d\n", total);
}

/**
 * This function is supposed to be somewhat like strcat, but it doesn't work.
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 *
 * One of the issues was that the buf didn't have a size assigned to it. I assigned it a size of 128
 * I also solved the problem of not being able to return a local variable by 
 */
char *
suffixer(char buf[], char *a, char *b)
{
  FUNC_START();
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdangling-pointer"

  strcpy(buf, a);
  strcpy((buf + strlen(a)), b);


  return buf;

  #pragma GCC diagnostic pop
}

/**
 * This is an excerpt of Elon Musk's Full Self Driving code. Unfortunately, it
 * keeps telling people to take the wrong turn. Figure out how to fix it, and
 * explain what's going on so Elon can get back to work on leaving Earth for
 * good.
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 *
 *   One of the problems was the max size of the character buffers since saint agnew exceeded the orignal size of 8. I expanded the size to 16
 *	 Another problem weas that memcpy wasn't the right method to use, strycopy was the right way to give street4 it's new name
 */
void
driver(void)
{
  FUNC_START();

  char street1[16] = { "fulton" };
  char street2[16] = { "gomery" };
  char street3[16] = { "piedmont" };
  char street4[16] = { "baker" };
  char street5[16] = { "haight" };

  if (strcmp(street1, street2)) {
    char *new_name = "saint agnew ";
    strcpy(street4, new_name);
  }

  printf("Welcome to TeslaOS 0.1!\n");
  printf("Calculating route...\n");
  printf("Turn left at the intersection of %s and %s.\n", street5, street3);
}

/**
 * This function tokenizes a string by space, sort of like a basic strtok or
 * strsep. It has two subtle memory bugs for you to find.
 *
 * (1) Fix the problem.
 * (2) Explain what's wrong with this code:
 * free didn't work 
 *
 *  There were two problems with this. The first one is on line 225, which was that the malloc didn't take into account space for a null character
 *  The second was that when free was used the pointer wasn't at the beginning, which for some reason causes free not to work. This was fixed by assigning a value to the same memory location of line
 *  so that the pointer stayed at the beginning
 
 */
void
tokenizer(void)
{
  FUNC_START();

  char *str = "Hope was a letter I never could send";
  char *line = malloc(strlen(str) + 1);
  char *backup = line;
  char *c = line;

  strcpy(line, str);

  while (*c != '\0') {

    for ( ; *c != ' ' && *c != '\0'; c++) {
      // find the next space (or end of string)
    }

    *c = '\0';
    printf("%s\n", line);

    line = c + 1;
    c = line;
  }

  free(backup);
}

/**
* This function should print one thing you like about C, and one thing you
* dislike about it. Assuming you don't mess up, there will be no bugs to find
* here!
*/
void
finisher(void)
{
  FUNC_START();

  printf("I like how interactive it is to debug c, especially using the layout provided by our systems");
  printf("I dislike the memory management aspect of c, that has been the most frustrating thing for me because I get the logic, but sometimes can't get the change memory that I desire when debugging.");
}

int
main(void)
{
  printf("Starting up!");

  angular();

  greeter();

  displayer();

  int nums[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
  int length = 10;
  adder(nums, length);
  char buf[128];
  char *result = suffixer(buf,"kayak", "ing");
  printf("Suffixed: %s\n", result);

  driver();

  tokenizer();

  finisher();

  return 0;
}
