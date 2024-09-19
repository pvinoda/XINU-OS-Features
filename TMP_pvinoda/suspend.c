/* suspend.c - suspend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  suspend  --  suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
SYSCALL suspend(int pid)
{
    STATWORD ps;    
    struct pentry *pptr;        /* pointer to proc. tab. entry */
    int prio;                   /* priority returned */
    unsigned long start_time = 0;

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadpid(pid) || pid == NULLPROC ||
        ((pptr = &proctab[pid])->pstate != PRCURR && pptr->pstate != PRREADY)) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  // Calculating the elapsed time
            int syscall_index = SYS_SUSPEND;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }
    if (pptr->pstate == PRREADY) {
        pptr->pstate = PRSUSP;
        dequeue(pid);
    } else {
        pptr->pstate = PRSUSP;
        resched();
    }
    prio = pptr->pprio;

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SUSPEND;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    restore(ps);
    return(prio);
}
