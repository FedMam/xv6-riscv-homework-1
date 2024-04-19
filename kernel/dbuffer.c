#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include <stdarg.h>

struct {
    int head_index;
    int tail_index;
    char buffer[DBSIZE];
    struct spinlock lock;
} dbuffer;

void
dbufferinit(void) {
    dbuffer.head_index = 0;
    dbuffer.tail_index = 0;
    initlock(&dbuffer.lock, "dbuffer_lock");
}

static void
dbufferputc_nolock(char c) {
    dbuffer.buffer[dbuffer.head_index] = c;

    dbuffer.head_index++;
    if (dbuffer.head_index >= DBSIZE) dbuffer.head_index = 0;

    if (dbuffer.head_index == dbuffer.tail_index) {
        dbuffer.tail_index++;
        if (dbuffer.tail_index >= DBSIZE) dbuffer.tail_index = 0;
    }
}

static char* digits = "0123456789abcdef";

static void
dbufferwriteint_nolock(int num) {
    if (num < 0) {
        dbufferputc_nolock('-');
        num = -num;
    }

    int pow10 = 1000000000;
    int first_digit_met = 0;
    while (pow10 > 0) {
        if (num / pow10 > 0 || first_digit_met || pow10 == 1) {
            dbufferputc_nolock(digits[num / pow10]);
            first_digit_met = 1;
        }

        num %= pow10;
        pow10 /= 10;
    }
}

static void
dbufferwritehex_nolock(uint64 num) {
    uint64 pow16 = 0x1000000000000000;
    int first_digit_met = 0;
    while (pow16 > 0) {
        if (num / pow16 > 0 || first_digit_met || pow16 == 1) {
            dbufferputc_nolock(digits[num / pow16]);
            first_digit_met = 1;
        }

        num %= pow16;
        pow16 /= 16;
    }
}

static void
dbufferwriteptr_nolock(uint64 x)
{
  int i;
  dbufferputc_nolock('0');
  dbufferputc_nolock('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    dbufferputc_nolock(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

void
dbufferwrite(const char* buf, uint num) {
    acquire(&dbuffer.lock);

    for (const char *p = buf; p < buf + num; ++p)
        dbufferputc_nolock(*p);

    release(&dbuffer.lock);
}

int
dbufferread(char* buf, uint num) {
    acquire(&dbuffer.lock);

    int size = dbuffer.head_index - dbuffer.tail_index;
    if (size < 0) size += DBSIZE;
    if (size > num) size = num;

    int start = dbuffer.head_index - size;
    if (start < 0) start += DBSIZE;

    for (int i = 0; i < size; i++) {
        char c = dbuffer.buffer[(start + i) % DBSIZE];

        buf[i] = c;
    }

    release(&dbuffer.lock);
    return size;
}

int
dbufferreadnterm(char* buf, uint num) {
    // reads N bytes from dbuffer, including the 0-terminator.
    if (num == 0) return 0;

    int size = dbufferread(buf, num - 1);
    buf[size] = 0;
    return size + 1;
}

int
dbuffercopyout(uint64 dstva, uint num) {
    if (num == 0) return 0;

    acquire(&dbuffer.lock);

    int size = dbuffer.head_index - dbuffer.tail_index;
    if (size < 0) size += DBSIZE;
    if (size > num - 1) size = num - 1; // there'll be also a null terminator at the end

    int start = dbuffer.head_index - size;
    if (start < 0) start += DBSIZE;

    int bytes_prefix = DBSIZE - start;
    if (bytes_prefix > size) bytes_prefix = size;
    int bytes_suffix = size - bytes_prefix;

    struct proc *p = myproc();
    copyout(p->pagetable, dstva, dbuffer.buffer + start, bytes_prefix);
    if (bytes_suffix > 0)
        copyout(p->pagetable, dstva + bytes_prefix, dbuffer.buffer, bytes_suffix);

    copyout(p->pagetable, dstva + size, "\0", 1);

    release(&dbuffer.lock);
    return size + 1;
}

void
pr_msg(const char *fmt, ...) {
    // code duplication from printf
    acquire(&dbuffer.lock);

    if (fmt == 0)
        panic("pr_msg: null fmt");

    dbufferputc_nolock('[');
    acquire(&tickslock);
    dbufferwriteint_nolock(ticks);
    release(&tickslock);
    dbufferputc_nolock(']');
    dbufferputc_nolock(' ');

    va_list ap;
    char *s;
    char c;
    va_start(ap, fmt);
    int last_perc = 0;
    for(int i = 0; (c = fmt[i]) != 0; i++) {
        if (last_perc) {
            if (c == 'd') {
                dbufferwriteint_nolock(va_arg(ap, int));
            }
            else if (c == 'x') {
                dbufferwritehex_nolock(va_arg(ap, uint64));
            }
            else if (c == 'p') {
                dbufferwriteptr_nolock(va_arg(ap, uint64));
            }
            else if (c == 's') {
                if((s = va_arg(ap, char*)) == 0)
                    s = "(null)";
                for(; *s; s++)
                    dbufferputc_nolock(*s);
            }
            else if (c == 'c') {
                // added this format for convenience
                dbufferputc_nolock(va_arg(ap, char));
            }
            else if (c == '%') {
                dbufferputc_nolock('%');
            }
            else {
                dbufferputc_nolock('%');
                dbufferputc_nolock(c);
            }
            last_perc = 0;
        }
        else {
            if (c == '%')
                last_perc = 1;
            else
                dbufferputc_nolock(c);
        }
    }

    dbufferputc_nolock('\n');

    va_end(ap);

    release(&dbuffer.lock);
}

// --
void
dbufferdebug(void) {
    printf("DBuffer stats:\nHead index: %d\nTail index: %d\n",
        dbuffer.head_index, dbuffer.tail_index);
    printf("Buffer as string: %s\nBuffer contents:\n", dbuffer.buffer);
    char *digits = "0123456789ABCDEF";
    for (int i = 0; i < DBSIZE / 32; i++) {
        for (int j = 0; j < 32; j++) {
            unsigned char c = dbuffer.buffer[i * 32 + j];
            consputc(digits[c / 16]);
            consputc(digits[c % 16]);
            consputc(' ');
        }
        consputc('\n');
    }
}
