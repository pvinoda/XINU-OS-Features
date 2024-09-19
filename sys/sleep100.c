/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep100(int n)
{
    STATWORD ps;    
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (n < 0 || clkruns == 0) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  // Calculating the elapsed time
            int syscall_index = SYS_SLEEP100;  // SYS_SLEEP100 defined in index enumeration
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }

    disable(ps);
    if (n == 0) {       // sleep100(0) -> end time slice
        ; // No operation but ends the time slice
    } else {
        insertd(currpid, clockq, n * 10); 
        slnempty = TRUE;
        sltop = &q[q[clockq].qnext].qkey;
        proctab[currpid].pstate = PRSLEEP;
    }
    resched();
    restore(ps);

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SLEEP100;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return(OK);
}
