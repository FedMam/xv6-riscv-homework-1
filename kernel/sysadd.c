#include "types.h"
#include "riscv.h"
#include "defs.h"

uint64 sys_add(void) {
    int num1 = 0, num2 = 0;
    char input_ch = '\0';

    for (int i = 0; i < 25; i++) {
        printf("%d\n", 25);
    }

    /*
    char buffer[25];

    char *buffer_read_pos = buffer;
    for (int i = 0; i < 25; i++) {
        if (read(0, buffer_read_pos, 1) != 1) {
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
    return 0;
    */
}