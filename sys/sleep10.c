/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
SYSCALL sleep10(int n)
{
    STATWORD ps;    
    unsigned long start_time = 0;

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (n < 0 || clkruns == 0) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;
            int syscall_index = SYS_SLEEP10;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }
    
    disable(ps);
    if (n == 0) {       // sleep10(0) -> end time slice
        ; // Do nothing but the slice ends
    } else {
        insertd(currpid, clockq, n * 100); // Convert tenths of seconds to system ticks
        slnempty = TRUE;
        sltop = &q[q[clockq].qnext].qkey;
        proctab[currpid].pstate = PRSLEEP;
    }
    resched();
    restore(ps);

    // Updating syscall statistics if tracing is active after execution
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SLEEP10;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return(OK);
}
