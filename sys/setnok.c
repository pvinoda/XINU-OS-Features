/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL setnok(int nok, int pid)
{
    STATWORD ps;    
    struct pentry *pptr;
    unsigned long start_time = 0;  // Variable to store the start time

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadpid(pid)) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  // Calculate the elapsed time
            int syscall_index = SYS_SETNOK;  // Index for setnok
            proctab[currpid].syscalls[syscall_index].count++;  // Increment the count
            proctab[currpid].syscalls[syscall_index].total_time += duration;  // Add the duration
        }
        restore(ps);
        return(SYSERR);
    }

    pptr = &proctab[pid];
    pptr->pnxtkin = nok;
    restore(ps);

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;  // Calculating the elapsed time
        int syscall_index = SYS_SETNOK;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return(OK);
}
