/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>

extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
SYSCALL	gettime(long *timvar)
{
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }
    /* long	now; */

	/* FIXME -- no getutim */
    

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        int syscall_index = SYS_GETTIME;  
        proctab[currpid].syscalls[syscall_index].count++;
        proctab[currpid].syscalls[syscall_index].total_time += duration;
    }


    return OK;
}
