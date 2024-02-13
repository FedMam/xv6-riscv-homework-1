#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char buffer[25];
   
  if (!gets(buffer, 25)) {
    // failure
    write(2, "sum: read failure\n", 23);
    exit(1);
  }
  
  char *ptr1 = buffer, *ptr2 = strchr(buffer, ' ');
  
  if (!ptr2) {
    // the string does not contain a space
    write(2, "sum: not enough args\n", 23);
    exit(1);
  }
  
  *ptr2++ = '\0'; // split string
  
  int num1 = atoi(ptr1), num2 = atoi(ptr2); // incorrect string converts to 0
  int sum = num1 + num2;
  
  printf("%d\n", sum);
  exit(0);
}
