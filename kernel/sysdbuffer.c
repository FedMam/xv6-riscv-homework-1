#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"

uint64
sys_dmesg(void) {
    uint64 dest;
    int num;

    argaddr(0, &dest);
    argint(1, &num);

    return dbuffercopyout(dest, num);
}