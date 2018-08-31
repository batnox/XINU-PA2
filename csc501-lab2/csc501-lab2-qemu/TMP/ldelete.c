/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lck)
{
	STATWORD ps;    
	int	pid;
	struct	lentry	*lptr;

	disable(ps);
	if (isbadlock(lck) || locks[lck].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	lptr = &locks[lck];
	lptr->lstate = LFREE;
	int flag = 0;
	int i=0;
	for(i=0; i<NPROC; i++){
		locks[lck].lock_holders[currpid] = -1;
	}
	if (nonempty(lptr->lwqhead)) {
		while( (pid=getfirst(lptr->lwqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		flag=1;
	}
	 if (nonempty(lptr->lrqhead)) {
                while( (pid=getfirst(lptr->lrqhead)) != EMPTY)
                  {
                    proctab[pid].pwaitret = DELETED;
                    ready(pid,RESCHNO);
                  }
		flag = 1;
        }
	if(flag==1){
                resched();

	}
	restore(ps);
	return(OK);
}
