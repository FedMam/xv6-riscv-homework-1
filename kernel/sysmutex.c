#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "mutex.h"

uint64
sys_newmtx(void)
{
  struct proc *p = myproc();

  int mutex_id = newmutex();
  if (mutex_id == -1)
    return -1;
  
  p->mutexused[mutex_id] = 1;
  return mutex_id;
}

uint64
sys_acqmtx(void)
{
  int mutex_id;
  argint(0, &mutex_id);

  return acquiremutex(mutex_id);
}

uint64
sys_relmtx(void)
{
  int mutex_id;
  argint(0, &mutex_id);

  return releasemutex(mutex_id);
}

uint64
sys_clsmtx(void)
{
  struct proc *p = myproc();
  int mutex_id;
  argint(0, &mutex_id);

  if (!p->mutexused[mutex_id])
    return -1;
  
  p->mutexused[mutex_id] = 0;
  return closemutex(mutex_id);
}
