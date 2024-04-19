#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char** argv) {
    int num;
    if (argc < 2) {
        printf("Usage: dmesgfill <number-of-execs>\n");
    }

    num = atoi(argv[1]);
    char* echoargv[1];
    echoargv[0] = "";
    
    for (int i = 0; i < num; i++) {
        int pid = fork();
        if (pid == -1) {
            printf("dmesgfill: fork failure\n");
            exit(1);
        }
        else if (pid == 0) {
            exec("echo", echoargv);
            break;
        }
        else {
            wait(&pid);
        }
    }

    exit(0);
}