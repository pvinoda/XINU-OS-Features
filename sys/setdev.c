/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	short	*nxtdev;
	unsigned long start_time = 0;  // Variable to store the start time

	// Start timing if syscall tracing is active
	if (proctab[currpid].syscall_active) {
		start_time = ctr1000;
	}

	if (isbadpid(pid)) {
		if (proctab[currpid].syscall_active) {
			unsigned long duration = ctr1000 - start_time;  // Calculate the elapsed time
			int syscall_index = SYS_SETDEV;  // Use the index for setdev, define it globally if not already defined
			proctab[currpid].syscalls[syscall_index].count++;  // Increment the count for this syscall
			proctab[currpid].syscalls[syscall_index].total_time += duration;  // Add the duration to the total time
		}
		return(SYSERR);
	}

	nxtdev = (short *)proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;

	if (proctab[currpid].syscall_active) {
		unsigned long duration = ctr1000 - start_time;  // Calculate the elapsed time
		int syscall_index = SYS_SETDEV;
		proctab[currpid].syscalls[syscall_index].count++;  // Increment the count for this syscall
		proctab[currpid].syscalls[syscall_index].total_time += duration;  // Add the duration to the total time
	}

	return(OK);
}
