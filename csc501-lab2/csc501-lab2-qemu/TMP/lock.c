/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * This function follows the rules of locking given in the PA. The arrays lock->lock_holers and proc->locks_held is used to keep track of mapping between locks and process as well as to know what has entered critical section of the locks. The first time a process acquires a lock we set the correspind values in both arrays to 0. When a process enters the critical section tje element is set to 1. Otherwise the array elements are -1.  
 *------------------------------------------------------------------------
 */

unsigned long   ctr1000; 
SYSCALL	lock(int lck, int ltype, int lpr)
{
	STATWORD ps;    
	struct	lentry	*lptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadlock(lck) || (lptr= &locks[lck])->lstate==LFREE || isnegative(locks[lck].lock_holders[currpid]) != isnegative(proctab[currpid].locks_held[lck])) {
		restore(ps);
		return(SYSERR);
	}
		
	int temp1 = q[rdytail].qprev;
       // while(temp1<NPROC){
       //         kprintf("%d ",temp1);
                temp1 = q[temp1].qprev;
        //}
	
	locks[lck].lock_holders[currpid]=0;
	proctab[currpid].locks_held[lck]=0;

	if (lptr->locktype == WRITE) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->plck = lck;
		pptr->plckprio = lpr;
		pptr->lcktime = ctr1000;
		//prioinh(lck, currpid);
		if(ltype == WRITE ){
			insert(currpid, lptr->lwqhead, lpr);
			
		}
		else{
			insert(currpid, lptr->lrqhead, lpr);
		}
		pptr->pwaitret = OK;
		
		proctab[currpid].lock_blocked=lck;
		prioinh(lck, currpid);
		
		resched();
		restore(ps);
		return pptr->pwaitret;
	}
	
	else if(lptr->locktype == READ){
		//kprintf("current lock READ currpid %d process name %s\n",currpid, proctab[currpid].pname);
		pptr = &proctab[currpid];
		pptr->plck = lck;
		pptr->plckprio = lpr;
		pptr->lcktime = ctr1000;
		if(ltype == READ){// && q[q[lptr->lwqtail].qprev].plckprio > lpr){
		//	kprintf("THESE READERS SHOULDN'T BE HERE\n");
			struct pentry *tempp;
			if(nonempty(lptr->lwqhead)){
				tempp = &proctab[q[lptr->lwqtail].qprev];
				if(tempp->plckprio > lpr){
					pptr->pstate = PRWAIT;
					insert(currpid, lptr->lrqhead, lpr);
					pptr->pwaitret = OK;
					
					proctab[currpid].lock_blocked=lck;
					prioinh(lck,currpid);
					
					resched();
					restore(ps);
					return pptr->pwaitret;
				}
			}
		}
			
		if(ltype == READ){// && (pptr=&(q[q[lptr->lwqtail].qprev])->plckprio <= lpr)){
			struct pentry *tempp;
			tempp = &(proctab[q[lptr->lwqtail].qprev]);
			if((nonempty(lptr->lwqhead) && tempp->plckprio <=lpr) || isempty(lptr->lwqhead)){ 
				lptr->num_readers++;
			//	printf("numreader in lock %d\n", lptr->num_readers);
				lptr->currproc = currpid;
				if(lpr > locks[lck].lock_prio){
					locks[lck].lock_prio = lpr;
				}
				
				lptr->lock_holders[currpid]=1;
				proctab[currpid].locks_held[lck] = 1;
				proctab[currpid].lock_blocked=-1;
				
				restore(ps);
				return(OK);
			}
		}		
		else if(ltype == WRITE){
			//kprintf("reached in write\n");
			pptr->pstate = PRWAIT;
			//kprintf("currstate %d and pptr state %d \n", proctab[currpid].pstate, pptr->pstate);
			 int temp = q[rdytail].qprev;
        //while(temp<NPROC){
               // kprintf("%d ",temp);
          //      temp = q[temp].qprev;
        //}

			insert(currpid, lptr->lwqhead, lpr);
//	kprintf("\n");
	 temp = q[rdytail].qprev;

        //while(temp<NPROC){
  //              kprintf("%d ",temp);
          //      temp = q[temp].qprev;
       // }

			
			pptr->pwaitret = OK;
			
			proctab[currpid].lock_blocked=lck;
			prioinh(lck, currpid);
//			kprintf("\nafter prioinh\n");		
			resched();
			restore(ps);
			return pptr->pwaitret;

		}
	
	}
	else if(lptr->locktype == NONE){
		pptr = &proctab[currpid];
		pptr->plck = lck;
		pptr->plckprio = lpr;
		pptr->lcktime = ctr1000;
	
		if(ltype == READ){
			lptr->num_readers++;
				//printf("numreader in lock %d\n", lptr->num_readers);
			lptr->currproc = currpid;
			if(lpr > locks[lck].lock_prio){
				locks[lck].lock_prio = lpr;
			}
			lptr->locktype = READ;
			
			lptr->lock_holders[currpid]=1;
			proctab[currpid].locks_held[lck] = 1;
			proctab[currpid].lock_blocked=-1;
			
			restore(ps);
			return(OK);
		}		
		else if(ltype == WRITE){
			//locks[lck]->num_readers++;
			lptr->currproc = currpid;
			if(lpr > locks[lck].lock_prio){
				locks[lck].lock_prio = lpr;
			}
			lptr->locktype = WRITE;
			
			lptr->lock_holders[currpid]=1;
			proctab[currpid].locks_held[lck] = 1;
			proctab[currpid].lock_blocked=-1;

			restore(ps);
			return(OK);

		}

	}
	restore(ps);
	return(OK);
}
