/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep1000(int n)
{
    STATWORD ps;    
    unsigned long start_time = 0;

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (n < 0 || clkruns == 0) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  // Calculate the elapsed time
            int syscall_index = SYS_SLEEP1000;  // SYS_SLEEP1000 in the syscall index enumeration
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }

    disable(ps);
    if (n == 0) {       // sleep1000(0) -> end time slice
        ; // No operation but ends the time slice
    } else {
        insertd(currpid, clockq, n * 1000); // Convert seconds to system ticks
        slnempty = TRUE;
        sltop = &q[q[clockq].qnext].qkey;
        proctab[currpid].pstate = PRSLEEP;
    }
    resched();
    restore(ps);

    // Update syscall statistics if tracing is active after execution
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SLEEP1000;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return(OK);
}
