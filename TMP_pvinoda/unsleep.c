/* unsleep.c - unsleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * unsleep  --  remove process from the sleep queue prematurely
 *------------------------------------------------------------------------
 */
SYSCALL unsleep(int pid)
{
    STATWORD ps;    
    struct pentry *pptr;
    struct qent *qptr;
    int remain;
    int next;
    unsigned long start_time = 0;

    // Start timing if syscall tracing is active
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    disable(ps);
    if (isbadpid(pid) || ( (pptr = &proctab[pid])->pstate != PRSLEEP &&
        pptr->pstate != PRTRECV)) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_UNSLEEP;  
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }
    qptr = &q[pid];
    remain = qptr->qkey;
    if ((next=qptr->qnext) < NPROC)
        q[next].qkey += remain;
    dequeue(pid);
    if ((next=q[clockq].qnext) < NPROC)
        sltop = (int *) &q[next].qkey;
    else
        slnempty = FALSE;
    
    // Update syscall statistics if tracing is active
    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_UNSLEEP;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    restore(ps);
    return(OK);
}