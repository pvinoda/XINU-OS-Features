/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
    STATWORD ps;    
    register struct sentry *sptr;
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadsem(sem) || (sptr = &semaph[sem])->sstate == SFREE) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_SIGNAL; 
            proctab[currpid].syscalls[syscall_index].count++;  
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }
    if ((sptr->semcnt++) < 0)
        ready(getfirst(sptr->sqhead), RESCHYES);
    restore(ps);

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SIGNAL;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return(OK);
}
