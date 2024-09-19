/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	unsigned long start_time = 0;

	if (proctab[currpid].syscall_active) {
		start_time = ctr1000;  
	}

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		if (proctab[currpid].syscall_active) {
			unsigned long duration = ctr1000 - start_time;
			int syscall_index = SYS_KILL;  
			proctab[currpid].syscalls[syscall_index].count++;
			proctab[currpid].syscalls[syscall_index].total_time += duration;
		}
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (!isbaddev(dev))
		close(dev);
	dev = pptr->pdevs[1];
	if (!isbaddev(dev))
		close(dev);
	dev = pptr->ppagedev;
	if (!isbaddev(dev))
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {
	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}

	if (proctab[currpid].syscall_active) {
		unsigned long duration = ctr1000 - start_time;  // Calculate duration
		int syscall_index = SYS_KILL;  // Index for kill syscall
		proctab[currpid].syscalls[syscall_index].count++;
		proctab[currpid].syscalls[syscall_index].total_time += duration;
	}

	restore(ps);
	return(OK);
}
