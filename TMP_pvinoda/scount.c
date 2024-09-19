/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>

extern struct sentry semaph[];

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
    STATWORD ps;    
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (isbadsem(sem) || semaph[sem].sstate == SFREE) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_SCOUNT; 
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_SCOUNT;
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }

    return(semaph[sem].semcnt);
}
