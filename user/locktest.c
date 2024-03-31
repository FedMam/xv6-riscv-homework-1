#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
  int use_lock;
  char* usage_msg = "locktest: usage: locktest <string> (without lock), locktest -l <string> (with lock)\n";
  char* test_string;
  if (argc <= 1) {
    printf(usage_msg);
    exit(1);
  }
  else if (argc == 2) {
    if (strcmp(argv[1], "-l") == 0) {
      printf(usage_msg);
      exit(1);
    }
    use_lock = 0;
    test_string = argv[1];
  }
  else {
    if (strcmp(argv[1], "-l") == 0) {
      use_lock = 1;
      test_string = argv[2];
    }
    else {
      use_lock = 0;
      test_string = argv[1];
    }
  }

  int mutex_id = -1;
  if (use_lock) {
    mutex_id = newmtx();
    if (mutex_id == -1) {
      printf("locktest: mutex init failure\n");
      exit(-1);
    }
  }

  int ctop_pipe[2];
  int ptoc_pipe[2];
  if (pipe(ctop_pipe) == -1)
  {
    printf("locktest: pipe failure\n");
    exit(-2);
  }
  if (pipe(ptoc_pipe) == -1)
  {
    printf("locktest: pipe failure\n");
    exit(-2);
  }

  int fork_pid = fork();
  if (fork_pid == -1) {
    printf("locktest: fork failure\n");
    exit(-3);
  }
  else if (fork_pid == 0) {
    // child process
    int pid = getpid();
    close(ptoc_pipe[1]);
    close(ctop_pipe[0]);

    char inp;
    while (1) {
        if (use_lock) acqmtx(mutex_id);
        int read_status = read(ptoc_pipe[0], &inp, 1);
        if (read_status == -1) {
            printf("task (child): read failure\n");
            close(ctop_pipe[1]);
            close(ptoc_pipe[0]);
            exit(-4);
        }
        if (read_status == 0 || inp == '\0') {
            break;
        }
        printf("%d: received '%c'\n", pid, inp);

        if (write(ctop_pipe[1], &inp, 1) == -1) {
          printf("task (child): write failure\n");
          close(ctop_pipe[1]);
          close(ptoc_pipe[0]);
          exit(-4);
        }
        if (use_lock) relmtx(mutex_id);
    }

    close(ctop_pipe[1]);
    close(ptoc_pipe[0]);
    exit(0);
  }
  else {
    // parent process
    int pid = getpid();
    close(ctop_pipe[1]);
    close(ptoc_pipe[0]);

    if (use_lock) acqmtx(mutex_id);
    if (write(ptoc_pipe[1], test_string, strlen(test_string) + 1) == -1) {
      printf("task (parent): write failure\n");
      close(ptoc_pipe[1]);
      close(ctop_pipe[0]);
      exit(-5);
    }
    if (use_lock) relmtx(mutex_id);

    close(ptoc_pipe[1]);

    char inp;
    while (1) {
        if (use_lock) acqmtx(mutex_id);
        int read_status = read(ctop_pipe[0], &inp, 1);
        if (read_status == -1) {
            printf("task (parent): read failure\n");
            close(ctop_pipe[0]);
            exit(-5);
        }
        if (read_status == 0 || inp == '\0') {
            break;
        }
        printf("%d: received '%c'\n", pid, inp);
        if (use_lock) relmtx(mutex_id);
    }

    close(ctop_pipe[0]);
    exit(0);
  }
}