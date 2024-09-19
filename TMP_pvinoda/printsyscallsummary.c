// printsyscallsummary.c

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

const char *syscall_names[] = {
    "sys_freemem",
    "sys_chprio",
    "sys_getpid",
    "sys_getprio",
    "sys_gettime",
    "sys_kill",
    "sys_receive",
    "sys_recvclr",
    "sys_recvtim",
    "sys_resume",
    "sys_scount",
    "sys_sdelete",
    "sys_send",
    "sys_setdev",
    "sys_setnok",
    "sys_screate",
    "sys_signal",
    "sys_signaln",
    "sys_sleep",
    "sys_sleep10",
    "sys_sleep100",
    "sys_sleep1000",
    "sys_sreset",
    "sys_stacktrace",
    "sys_suspend",
    "sys_unsleep",
    "sys_wait"
};


void syscallsummary_start() {
    int i, j;
    for (i = 0; i < NPROC; i++) {
        proctab[i].syscall_active = 1;
        for (j=0; j < NUM_SYS_CALLS; j++){
            proctab[i].syscalls[j].count = 0;
            proctab[i].syscalls[j].total_time = 0;
        }
    }
}

void syscallsummary_stop() {
    int i;
    for (i = 0; i < NPROC; i++) {
        proctab[i].syscall_active = 0;
    }
}

void printsyscallsummary() {
    int i, j;

    for (i=0; i < NPROC; i++) {
        int systemCallPresent = 0;  // Reset flag for each process

        // First, check if any system call has been made by this process
        for (j=0; j < NUM_SYS_CALLS; j++) {
            if (proctab[i].syscalls[j].count > 0) {
                systemCallPresent = 1;
                break;  
            }
        }

        // If any system call is present, print details
        if (systemCallPresent) {
            kprintf("Process [pid:%d]\n", i);
            for (j=0; j < NUM_SYS_CALLS; j++) {
                if (proctab[i].syscalls[j].count > 0) {
                    kprintf("   Syscall: %s, count: %d, average execution time: %lu (ms)]\n",
                            syscall_names[j],
                            proctab[i].syscalls[j].count,
                            proctab[i].syscalls[j].total_time / proctab[i].syscalls[j].count);
                }
            }
        }
    }
}



