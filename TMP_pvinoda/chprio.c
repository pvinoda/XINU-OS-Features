/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
    STATWORD ps;    
    struct pentry *pptr;
    unsigned long start_time = 0;  

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000; 
    }

    disable(ps);
    if (isbadpid(pid) || newprio <= 0 ||
        (pptr = &proctab[pid])->pstate == PRFREE) {
        restore(ps);
        if (proctab[currpid].syscall_active) {
            // Even if it's an error, still logging the attempt and time taken
            unsigned long duration = ctr1000 - start_time;
            int syscall_index = SYS_CHPRIO;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }
    pptr->pprio = newprio;
    restore(ps);

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;  
        int syscall_index = SYS_CHPRIO;
        proctab[currpid].syscalls[syscall_index].count++;  
        proctab[currpid].syscalls[syscall_index].total_time += duration; 
    }

    return(newprio);
}
