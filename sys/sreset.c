/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL sreset(int sem, int count)
{
    STATWORD ps;    
    struct sentry *sptr;
    int pid;
    int slist;
    unsigned long start_time = 0;

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadsem(sem) || count < 0 || semaph[sem].sstate == SFREE) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  // Calculating the elapsed time
            int syscall_index = SYS_SRESET;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }
    sptr = &semaph[sem];
    slist = sptr->sqhead;
    while ((pid = getfirst(slist)) != EMPTY)
        ready(pid, RESCHNO);
    sptr->semcnt = count;
    resched();

    // Updating syscall statistics if tracing is active after execution
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SRESET;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    restore(ps);
    return(OK);
}
