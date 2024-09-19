/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
	STATWORD ps;
	unsigned long start_time = 0;  // Start time for the syscall

	// Start timing if syscall tracing is active
	if (proctab[currpid].syscall_active) {
		start_time = ctr1000;
	}

	if (n < 0 || clkruns == 0)
		return(SYSERR);
	if (n == 0) {
		disable(ps);
		resched();
		restore(ps);

		// Updating syscall statistics if tracing is active
		if (proctab[currpid].syscall_active) {
			unsigned long duration = ctr1000 - start_time;
			int syscall_index = SYS_SLEEP;  
			proctab[currpid].syscalls[syscall_index].count++;
			proctab[currpid].syscalls[syscall_index].total_time += duration;
		}

		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);

	if (proctab[currpid].syscall_active) {
		unsigned long duration = ctr1000 - start_time;
		int syscall_index = SYS_SLEEP;
		proctab[currpid].syscalls[syscall_index].count++;
		proctab[currpid].syscalls[syscall_index].total_time += duration;
	}

	return(OK);
}
