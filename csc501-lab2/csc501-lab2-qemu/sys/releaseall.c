#include <conf.h>
#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<q.h>
#include<io.h>
#include<stdio.h>

/*
Releasall calls function release to release each lock one by one. The function return 0 or 1 based on whether it faces any error or not. A variable in releaseall keeps multiplying all the return values. If any of them would be 0, the final output will be 0 and releaseall will return 0. The release function wakes up a waiting process only when the number of readers is 1 or if the leaving process is a Writer. The function follows the conditions of PA in performing the action
*/

unsigned long   ctr1000; 

LOCAL int release(int lck);

SYSCALL releaseall(nargs, args)
	int nargs;
	long args;
{
	STATWORD ps;
	disable(ps);
	unsigned long *a;

	a = (unsigned long *)(&args) + (nargs - 1);
//	kprintf("release args %d\n", *(&args+1));
	int lck;
	int flag = 1;
	int y=nargs;
	for(y=nargs; y > 0; y--){
		//lck = (int)(*a);
	//	kprintf("Iterator\n");
		lck = *(&args+(y-1));
	//	kprintf("lck %d\n", lck);
		flag  = flag * release(lck);
		*a--;
	}
	//kprintf("value of flag %d proc ID\n", flag, currpid);
	if(flag == 1){
		restore(ps);
		return(OK);
	}
	else{
		restore(ps);
		return SYSERR;
	}
}


LOCAL int release(int lck){
	//kprintf("release lock id %d\n", lck);
	STATWORD ps;
        struct lentry *lptr;
        struct pentry *pptr;
	int readwrite;
	int writerprio;
	unsigned long writertime;

        //disable(ps);

         if (isbadlock(lck) || (lptr= &locks[lck])->lstate==LFREE || locks[lck].lock_holders[currpid]!=1 || isnegative(locks[lck].lock_holders[currpid]) != isnegative(proctab[currpid].locks_held[lck])) {
          //     kprintf("RETURN 0\n");
		 //restore(ps);
                return 0;
        }

        else if(lptr->locktype == WRITE || (lptr->locktype == READ && --(lptr->num_readers)<1)){
	//	kprintf("type %d num readers-1 %d\n",lptr->locktype, lptr->num_readers);
		lptr->locktype = NONE;
		lptr->num_readers = 0;	
		
		lptr->lock_holders[currpid]=0;
		proctab[currpid].locks_held[lck]=0;
		
		//kprintf("type %d num readers-1 %d\n",lptr->locktype, lptr->num_readers);
		if(isempty(lptr->lrqhead) && isempty(lptr->lwqhead)){
			lptr->locktype = NONE;
			lptr->lock_prio = MININT;
			
			lmaxprio(lck);
			//restore(ps);
			return 1;
		}
		else if((isempty(lptr->lrqhead) && nonempty(lptr->lwqhead)) || (nonempty(lptr->lrqhead) && (proctab[q[lptr->lwqtail].qprev].plckprio > proctab[q[lptr->lrqtail].qprev].plckprio))){
			
			pptr = &proctab[q[lptr->lwqtail].qprev];	
	//		kprintf("release lock for writer %d\n",q[lptr->lwqtail].qprev );
			lptr->locktype = WRITE;
			lptr->lock_prio = pptr->plckprio;
			lptr->currproc = q[lptr->lwqtail].qprev;
			int thisproc = getlast(lptr->lwqtail);
			lptr->lock_holders[thisproc]=1;
	                proctab[thisproc].locks_held[lck]=1;
			proctab[thisproc].lock_blocked = -1;
	//		kprintf("before lmaxprio\n");
			lmaxprio(lck);
	//		kprintf("after lmaxprio\n");
			ready(thisproc, RESCHYES);
			//lmaxprio(lck);
			//restore(ps);
			return 1;
		}
		
		else{
			//kprintf("C release here\n");
			if(!(isempty(lptr->lrqhead)) && isempty(lptr->lwqhead) ){
				//pptr = &q[q[lptr->lrqtail].qprev]i
				//kprintf("C in first if condition\n");
				writerprio = MININT;
				writertime = MAXINT;
			}
			else{
				writerprio = proctab[q[lptr->lwqtail].qprev].plckprio;
				writertime = proctab[q[lptr->lwqtail].qprev].lcktime;
			}
			
			int temp;
			temp = q[lptr->lrqtail].qprev;
			//kprintf("pid of E %d", temp);
			while(temp > 0 && temp < NPROC){		
				if(proctab[temp].plckprio > writerprio){
					lptr->locktype = READ;
					lptr->num_readers++;
					int temproc = getlast(lptr->lrqtail);
					proctab[temproc].lock_blocked = -1;
					ready(temproc, RESCHYES);
				}

				else if(proctab[temp].plckprio = writerprio && (proctab[temp].lcktime - writertime)<500){
					lptr->locktype = READ;
					lptr->num_readers++;
					int temproc = getlast(lptr->lrqtail);
					proctab[temproc].lock_blocked = -1;
					ready(temproc, RESCHYES);
					//ready(getlast(lptr->lrqtail), RESCHYES);
					
				}
				else{
					break;
				}
				temp = q[temp].qprev;	
			}
			lmaxprio(lck);
			return 1;
		}

	}
		
		lptr->lock_holders[currpid]=0;
		proctab[currpid].locks_held[lck]=0;
		
		return 1;	
}
