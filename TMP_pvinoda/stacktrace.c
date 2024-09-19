/* stacktrace.c - stacktrace */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long *esp;
static unsigned long *ebp;

#define STKDETAIL
#define OK 1  // Assuming OK is not defined, define it locally or ensure it's included from the right header

/*------------------------------------------------------------------------
 * stacktrace - print a stack backtrace for a process
 *------------------------------------------------------------------------
 */
SYSCALL stacktrace(int pid)
{
    STATWORD ps;    
    unsigned long *sp, *fp = NULL;
    unsigned long start_time = 0;
    int syscall_index = SYS_STACKTRACE; // Declare here, use below consistently

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (pid != 0 && isbadpid(pid)) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return SYSERR;
    }

    struct pentry *proc = &proctab[pid];

    if (pid == currpid) {
        asm("movl %esp, esp");
        asm("movl %ebp, ebp");
        sp = esp;
        fp = ebp;
    } else {
        sp = (unsigned long *)proc->pesp;
        fp = sp + 2; // where ctxsw leaves it
    }

    disable(ps);
#ifdef STKDETAIL
    while (sp < (unsigned long *)proc->pbase) {
        for (; sp < fp; sp++) {
            kprintf("DATA (%08X) %08X (%d)\n", sp, *sp, *sp);
        }
        if (*sp == MAGIC) {
            break;
        }
        kprintf("\nFP   (%08X) %08X (%d)\n", sp, *sp, *sp);
        fp = (unsigned long *)*sp++;
        if (fp <= sp) {
            if (proctab[currpid].syscall_active) {
                unsigned long duration = ctr1000 - start_time;
                proctab[currpid].syscalls[syscall_index].count++;
                proctab[currpid].syscalls[syscall_index].total_time += duration;
            }
            restore(ps);
            return SYSERR;
        }
        sp++;
    }
#endif
    resched();
    restore(ps);

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return OK; // Make sure to return OK if everything is fine
}
