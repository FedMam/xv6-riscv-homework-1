#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "param.h"
#include "mutex.h"

struct {
  struct spinlock lock;
  struct mutex mutexes[NMUTEX];
} mtable;

void
mutexinit(void) {
  initlock(&mtable.lock, "mtable");
  
  for (struct mutex *mtx = mtable.mutexes; mtx < mtable.mutexes + NMUTEX; mtx++) {
    initsleeplock(&mtx->lock, "mutex lock");
    mtx->used_by = 0;
  }
}

int
newmutex() 
{
  acquire(&mtable.lock);
  for (uint mutex_id = 0; mutex_id < NMUTEX; mutex_id++) {
    if (mtable.mutexes[mutex_id].used_by == 0) {
      mtable.mutexes[mutex_id].used_by++;
      release(&mtable.lock);
      return mutex_id;
    }
  }
  release(&mtable.lock);
  return -1;
}

int
usemutex(int mutex_id) {
  if (mutex_id < 0 || mutex_id >= NMUTEX)
    return -1;
  
  acquire(&mtable.lock);
  mtable.mutexes[mutex_id].used_by++;
  release(&mtable.lock);
  
  return 0;
}

int
acquiremutex(int mutex_id) 
{
  if (mutex_id < 0 || mutex_id >= NMUTEX)
    return -1;

  struct mutex *mtx = &mtable.mutexes[mutex_id];
  acquiresleep(&mtx->lock);
  return 0;
}

int
releasemutex(int mutex_id) 
{
  if (mutex_id < 0 || mutex_id >= NMUTEX)
    return -1;

  struct mutex *mtx = &mtable.mutexes[mutex_id];
  releasesleep(&mtx->lock);
  return 0;
}

int
holdingmutex(int mutex_id) 
{
  if (mutex_id < 0 || mutex_id >= NMUTEX)
    return 0;

  struct mutex *mtx = &mtable.mutexes[mutex_id];
  return holdingsleep(&mtx->lock);
}

int
closemutex(int mutex_id) 
{
  if (mutex_id < 0 || mutex_id >= NMUTEX)
    return -1;
  
  acquire(&mtable.lock);
  if (mtable.mutexes[mutex_id].used_by == 0) {
    release(&mtable.lock);

    return -1;
  }
  else {
    mtable.mutexes[mutex_id].used_by--;
    if (holdingmutex(mutex_id))
      releasemutex(mutex_id);
    release(&mtable.lock);
    return 0;
  }
}
