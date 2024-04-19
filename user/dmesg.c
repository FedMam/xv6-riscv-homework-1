#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    // Количество байт, которое программа скопирует из
    // буфера, включая 0-терминал
    uint size = 4096 * 4;
    if (argc > 1) {
        int arg_size = atoi(argv[1]);
        if (arg_size >= 1 && arg_size < size)
            size = arg_size;
    }

    char* buf = malloc(size);
    
    dmesg(buf, size);

    // это вместо printf, который может зависнуть
    for (char* p = buf; *p != 0; p++) {
        char singlechar[] = {*p, 0};
        printf(singlechar);
    }
    free(buf);
    exit(0);
}