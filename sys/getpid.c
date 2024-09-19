/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
    unsigned long start_time = 0;  
    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;  
    }

    int pid = currpid;  

    if (proctab[currpid].syscall_active) {

        unsigned long duration = ctr1000 - start_time;  
        int syscall_index = SYS_GETPID; 

        proctab[currpid].syscalls[syscall_index].count++;  // Incrementing the count for this syscall
        proctab[currpid].syscalls[syscall_index].total_time += duration;  // Adding the duration to the total time
    }

    return pid;  
}
