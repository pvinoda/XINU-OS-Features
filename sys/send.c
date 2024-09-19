/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(int pid, WORD msg)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	unsigned long start_time = 0;  

	if (proctab[currpid].syscall_active) {
		start_time = ctr1000;
	}

	disable(ps);
	if (isbadpid(pid) || ((pptr= &proctab[pid])->pstate == PRFREE) || pptr->phasmsg != 0) {
		if (proctab[currpid].syscall_active) {
			unsigned long duration = ctr1000 - start_time;  
			int syscall_index = SYS_SEND;  
			proctab[currpid].syscalls[syscall_index].count++;
			proctab[currpid].syscalls[syscall_index].total_time += duration;
		}
		restore(ps);
		return(SYSERR);
	}
	pptr->pmsg = msg;
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV) {	// if receiver waits, start it
		ready(pid, RESCHYES);
	} else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	restore(ps);

	if (proctab[currpid].syscall_active) {
		unsigned long duration = ctr1000 - start_time;  // Calculating the elapsed time
		int syscall_index = SYS_SEND;
		proctab[currpid].syscalls[syscall_index].count++;
		proctab[currpid].syscalls[syscall_index].total_time += duration;
	}

	return(OK);
}
