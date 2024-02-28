#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int BUFFER_LEN = 25;

int
main(int argc, char *argv[])
{
  char buffer[25];

  char *buffer_read_pos = buffer;
  for (int i = 0; i < 25; i++) {
    int read_response = read(0, buffer_read_pos, 1);
    if (read_response == 0)
      break;
    else if (read_response < 0) {
      // failure
      write(2, "sum: read failure\n", 23);
      exit(1);
    }

    if (*buffer_read_pos == '\0' ||
        *buffer_read_pos == '\n' ||
        *buffer_read_pos == '\r')
      break;

    buffer_read_pos++;
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
