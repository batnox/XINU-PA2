/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/
	int tempprio;
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR)){
		tempprio = optr->pprio > optr->pinh ? optr->pprio : optr->pinh; 
		
		if( (lastkey(rdytail)<tempprio)) {
	//	kprintf("reshced first condition\n");
			return(OK);
		}
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
//		kprintf("resched second condition %d \n", currpid);
		optr->pstate = PRREADY;
		if(optr->pprio > optr->pinh)
			tempprio = optr->pprio;
		else
			tempprio = optr->pinh;
		//tempprio = optr->pprio > optr->pinh ? optr->pprio : optr->pinh;
//		kprintf("%d\n",tempprio );	
		 
		insert(currpid,rdyhead,tempprio);
	}

	/* remove highest priority process at end of ready list */
//	kprintf("previous currpid %d\n",currpid);
		//kprintf("%d\n",tempprio );	

		//if(optr->pprio > optr->pinh)
		//	tempprio = optr->pprio;
		//else
		//	tempprio = optr->pinh;
int temp = q[rdytail].qprev;
	while(temp<NPROC){
	//	kprintf("proc %d ",temp);	
	//kprintf("%d ",proctab[temp].pprio>proctab[temp].pinh?proctab[temp].pprio:proctab[temp].pinh);
		temp = q[temp].qprev;
	}
	//kprintf("\n=========RESCHED===========\n");
	 int temp1 = q[rdytail].qprev;
        while(temp1<NPROC){
  //              kprintf("%d ",temp1);
                temp1 = q[temp1].qprev;
        }
//	kprintf("\n========================\n");


	nptr = &proctab[ (currpid = getlast(rdytail)) ];
/*	kprintf("\n");
	temp = q[rdytail].qprev;
	while(temp<NPROC){
		kprintf("%d ",temp);
		temp = q[temp].qprev;
	}*/
//	kprintf("\nnew currpid %d\n",currpid);
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
		return OK;
}
