#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int pid = fork();

    if (pid < 0) 
    {
        printf("task: fork failure\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // in child process
        sleep(10);
        exit(1);
    }
    else
    {
        // in parent process
        int exit_status;
        pid = wait(&exit_status);
        printf("child process %d finished with exit code %d\n", pid, exit_status);
    }

    exit(0);
}