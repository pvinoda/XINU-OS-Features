/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <stdio.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
SYSCALL freemem(struct mblock *block, unsigned size)
{
    STATWORD ps;    
    struct mblock *p, *q;
    unsigned top;
    unsigned long start_time = 0;

    if (proctab[currpid].syscall_active) {
        start_time = ctr1000;
    }

    if (size == 0 || (unsigned)block > (unsigned)maxaddr
        || ((unsigned)block) < ((unsigned) &end)) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;  
            int syscall_index = SYS_FREEMEM;
            proctab[currpid].syscalls[syscall_index].count++;
            proctab[currpid].syscalls[syscall_index].total_time += duration;
        }
        return(SYSERR);
    }

    size = (unsigned)roundmb(size);
    disable(ps);
    for (p = memlist.mnext, q = &memlist;
         p != (struct mblock *) NULL && p < block;
         q = p, p = p->mnext);

    if (((top = q->mlen + (unsigned)q) > (unsigned)block && q != &memlist) ||
        (p != NULL && (size + (unsigned)block) > (unsigned)p)) {
        if (proctab[currpid].syscall_active) {
            unsigned long duration = ctr1000 - start_time;
            proctab[currpid].syscalls[SYS_FREEMEM].count++;
            proctab[currpid].syscalls[SYS_FREEMEM].total_time += duration;
        }
        restore(ps);
        return(SYSERR);
    }

    if (q != &memlist && top == (unsigned)block) {
        q->mlen += size;
    } else {
        block->mlen = size;
        block->mnext = p;
        q->mnext = block;
        q = block;
    }

    if ((unsigned)(q->mlen + (unsigned)q) == (unsigned)p) {
        q->mlen += p->mlen;
        q->mnext = p->mnext;
    }

    if (proctab[currpid].syscall_active) {
        unsigned long duration = ctr1000 - start_time;
        proctab[currpid].syscalls[SYS_FREEMEM].count++;
        proctab[currpid].syscalls[SYS_FREEMEM].total_time += duration;
    }

    restore(ps);
    return(OK);
}
