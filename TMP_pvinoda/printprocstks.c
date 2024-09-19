#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

void printprocstks(int priority) {
    struct pentry *proc;
    int i;

    for (i = 0; i < NPROC; i++) {
        proc = &proctab[i];
        if (proc->pstate != PRFREE && proc->pprio > priority) {
            kprintf("Process [%s]\n", proc->pname);
            kprintf("    pid: %d\n", i);
            kprintf("    priority: %d\n", proc->pprio);
            kprintf("    base: 0x%08X\n", proc->pbase);
            kprintf("    limit: 0x%08X\n", proc->plimit);
            kprintf("    len: %d\n", proc->pstklen);
            if (i == currpid) {
                unsigned long *sp;
                asm("movl %%esp, %0" : "=r"(sp)); // Capturing the current esp
                kprintf("    pointer: 0x%08X\n", sp);
            } else {
                kprintf("    pointer: 0x%08X\n", proc->pesp);
            }
        }
    }
}
