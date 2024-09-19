/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL wait(int sem)
{
    STATWORD ps;    
    struct sentry *sptr;
    struct pentry *pptr;
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadsem(sem) || (sptr = &semaph[sem])->sstate == SFREE) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_WAIT;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }
    
    if (--(sptr->semcnt) < 0) {
        (pptr = &proctab[currpid])->pstate = PRWAIT;
        pptr->psem = sem;
        enqueue(currpid, sptr->sqtail);
        pptr->pwaitret = OK;
        resched();
        restore(ps);
        return pptr->pwaitret;
    }
    
    // Updating the syscall statistics if tracing is active after successful execution
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_WAIT;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    restore(ps);
    return(OK);
}
