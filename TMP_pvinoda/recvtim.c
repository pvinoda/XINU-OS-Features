/* recvtim.c - recvtim */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
SYSCALL recvtim(int maxwait)
{
    STATWORD ps;    
    struct pentry *pptr;
    int msg;
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (maxwait < 0 || clkruns == 0) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_RECVTIM;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }

    disable(ps);
    pptr = &proctab[currpid];
    if (!pptr->phasmsg) {  // if no message, wait
        insertd(currpid, clockq, maxwait * 1000);
        slnempty = TRUE;
        sltop = (int *)&q[q[clockq].qnext].qkey;
        pptr->pstate = PRTRECV;
        resched();
    }

    if (pptr->phasmsg) {
        msg = pptr->pmsg;  // msg arrived => retrieve it
        pptr->phasmsg = FALSE;
    } else {  // still no message => TIMEOUT
        msg = TIMEOUT;
    }

    // Update syscall statistics if tracing is active
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_RECVTIM;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    restore(ps);
    return(msg);
}
