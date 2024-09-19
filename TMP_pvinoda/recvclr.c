/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{
	STATWORD ps;    
	WORD	msg;
	unsigned long start_time = 0;

	if (proctab[currpid].syscall_active){
		start_time = ctr1000;
	}

	disable(ps);
	if (proctab[currpid].phasmsg) {
		proctab[currpid].phasmsg = 0;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);

	// Update syscall statistics if tracing is active
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;  
        int syscall_index = SYS_RECVCLR;  // SYS_RECVCLR defined somewhere globally, perhaps in an enum or define list

        proctab[currpid].syscalls[syscall_index].count++;  // Incrementing the count for this syscall
        proctab[currpid].syscalls[syscall_index].total_time += duration;  // Adding the duration to the total time
    }

	return(msg);
}
