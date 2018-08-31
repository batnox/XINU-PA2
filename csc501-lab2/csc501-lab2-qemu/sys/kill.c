/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include<lock.h>
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 * When a waiting process is killed, we remove it from the queue and call lmaxprio for the affected lock
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}

	int flag=0;
	int temppid;
	if(pptr->lock_blocked != -1){
		int tempptr= q[locks[pptr->lock_blocked].lwqhead].qnext;
		while(tempptr != locks[pptr->lock_blocked].lwqtail){
			if(pid == tempptr){
				temppid = tempptr;
				  q[q[temppid].qprev].qnext = q[temppid].qnext;
                        q[q[temppid].qnext].qprev = q[temppid].qprev;
				lmaxprio(pptr->lock_blocked);
				pptr->lock_blocked = -1;
				flag=1;
				break;
			}
			tempptr= q[tempptr].qnext;
		}
		if(flag!=1){
		tempptr= q[locks[pptr->lock_blocked].lrqhead].qnext;
		while(tempptr != locks[pptr->lock_blocked].lrqtail){
			if(pid == tempptr){
				temppid = tempptr;
 			 q[q[temppid].qprev].qnext = q[temppid].qnext;
                        q[q[temppid].qnext].qprev = q[temppid].qprev;
				lmaxprio(pptr->lock_blocked);
				pptr->lock_blocked=-1;
				flag=1;
				break;
			}
			tempptr= q[tempptr].qnext;
		}
			
		}
		/*if(flag==1){
			dequeue(temppid);
			lmaxprio(pptr->lock_blocked);
			
		}*/
	}

	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
