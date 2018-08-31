/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	register struct	pentry	*pptr;
	int prio;
	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	if(pptr->pprio>pptr->pinh){
		prio = pptr->pprio;
	}
	else{
		prio = pptr->pinh;
	}
	insert(pid,rdyhead,prio);
	if (resch)
		resched();
	return(OK);
}
