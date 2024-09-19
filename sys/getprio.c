/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
    STATWORD ps;    
    struct pentry *pptr;
    unsigned long start_time = 0;  // Variable to store the start time

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;  // Get the current system time in milliseconds
    }

    disable(ps);
    if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
        restore(ps);
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_GETPRIO;  
            proctab[currpid].syscalls[syscall_index].count++;  
            proctab[currpid].syscalls[syscall_index].total_time += duration;  
        }
        return(SYSERR);
    }
    restore(ps);

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time; 
        int syscall_index = SYS_GETPRIO;  
        proctab[currpid].syscalls[syscall_index].count++;  
        proctab[currpid].syscalls[syscall_index].total_time += duration;  
    }

    return(pptr->pprio);  // Return the priority of the process
}
