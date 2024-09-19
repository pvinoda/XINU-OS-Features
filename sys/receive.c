/* receive.c - receive */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  receive  -  wait for a message and return it
 *------------------------------------------------------------------------
 */
SYSCALL	receive()
{
	STATWORD ps;    
	struct pentry *pptr;
	WORD msg;
	unsigned long start_time = 0;  // Variable to store the start time

	// Start timing if syscall tracing is active
	if (proctab[currpid].syscall_active) {
		start_time = ctr1000;
	}

	disable(ps);
	pptr = &proctab[currpid];
	if (!pptr->phasmsg) {		/* if no message, wait for one	*/
		pptr->pstate = PRRECV;
		resched();
	}
	msg = pptr->pmsg;		/* retrieve message		*/
	pptr->phasmsg = FALSE;
	restore(ps);

	// Update syscall statistics if tracing is active
	if (proctab[currpid].syscall_active) {
		unsigned long duration = ctr1000 - start_time;  // Calculating the elapsed time
		int syscall_index = SYS_RECEIVE;  // SYS_RECEIVE defined in syscall index enumeration
		proctab[currpid].syscalls[syscall_index].count++; 
		proctab[currpid].syscalls[syscall_index].total_time += duration;  
	}

	return msg;
}
