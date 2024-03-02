#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[])
{
    int pipe_fd[2];
    
    if (pipe(pipe_fd) == -1)
    {
        printf("task: pipe failure\n");
        exit(1);
    }
    
    int pid = fork();

    if (pid == -1)
    {
        printf("task: fork failure\n");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        exit(1);
    }
    else if (pid == 0)
    {
        // in parent process
        close(pipe_fd[0]);

        char lf = '\n';
        for (int i = 0; i < argc; i++) 
        {
            write(pipe_fd[1], argv[i], strlen(argv[i]));
            write(pipe_fd[1], &lf, 1);
        }

        close(pipe_fd[1]);
        wait(0);
        exit(0);
    }
    else 
    {
        // in child process
        close(pipe_fd[1]);

        char inp;
        while (1)
        {
            int read_status = read(pipe_fd[0], &inp, 1);
            if (read_status == -1)
            {
                printf("task (child): read failure\n");
                exit(1);
            }
            if (read_status == 0 || inp == '\0')
            {
                break;
            }
            printf("%c", inp);
        }

        close(pipe_fd[0]);
        exit(0);
    }
}