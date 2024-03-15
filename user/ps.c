#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int MAXLIMIT = 4096;

int
main(int argc, char** argv)
{
  int auto_lim = 0;

  if (argc < 2)
  {
    auto_lim = 1;
  }

  static char *states[] = {
  [UNUSED]    "UNUSED",
  [USED]      "USED",
  [SLEEPING]  "SLEEPING",
  [RUNNABLE]  "RUNNABLE",
  [RUNNING]   "RUNNING",
  [ZOMBIE]    "ZOMBIE"
  };

  struct procinfo *plist;

  int lim = 1;
  int result = -1;
  
  if (!auto_lim)
  {
    lim = atoi(argv[1]);
    if (lim > MAXLIMIT)
    {
      printf("ps: maximum limit for procinfo is %d\n", MAXLIMIT);
      exit(1);
    }
    plist = malloc(sizeof(struct procinfo) * lim);
    result = ps_listinfo(plist, lim);
  }
  else
  {
    lim = 1;
    plist = malloc(sizeof(struct procinfo) * lim);
    while ((result = ps_listinfo(plist, lim)) > lim)
    {
      free(plist);
      lim *= 2;
      plist = malloc(sizeof(struct procinfo) * lim);
    }
  }

  if (result > lim)
  {
    printf("ps: too many processes running\n");
    free(plist);
    exit(-1);
  }
  else if (result < 0)
  {
    printf("ps: write failure\n");
    free(plist);
    exit(-1);
  }
  else
  {
    for (struct procinfo *p = plist; p < &plist[result]; p++)
    {
      printf("%d: name: \'%s\', state: %s, parent: ",
        p->pid, p->name, states[p->state]);
      if (p->parent_pid == -1)
        printf("no parent\n");
      else printf("%d\n", p->parent_pid);
    }
    printf("%d processes total\n", result);
  }

  free(plist);
  exit(0);
}