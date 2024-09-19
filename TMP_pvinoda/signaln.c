/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
SYSCALL signaln(int sem, int count)
{
    STATWORD ps;    
    struct sentry *sptr;
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadsem(sem) || semaph[sem].sstate == SFREE || count <= 0) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_SIGNALN;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }
    sptr = &semaph[sem];
    for (; count > 0; count--) {
        if ((sptr->semcnt++) < 0) {
            ready(getfirst(sptr->sqhead), RESCHNO);
        }
    }
    resched();

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SIGNALN;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    restore(ps);
    return(OK);
}
