#include <conf.h>
#include<kernel.h>
#include<proc.h>
#include<lock.h>
#include<q.h>
#include<io.h>
#include<stdio.h>

unsigned long   ctr1000; 

LOCAL int release(int lck);

SYSCALL releaseall(nargs, args)
	int nargs;
	long args;
{

	unsigned long *a;

	a = (unsigned long *)(&args) + (nargs - 1);
//	kprintf("release args %d\n", *(&args+1));
	int lck;
	int flag = 1;
	for(; nargs > 0; nargs--){
		//lck = (int)(*a);
		lck = *(&args+(nargs-1));
		flag  = flag * release(lck);
		*a--;
	}
	if(flag == 1){
		return(OK);
	}
	else{
		return SYSERR;
	}
}


LOCAL int release(int lck){
	kprintf("release lock id %d\n", lck);
	STATWORD ps;
        struct lentry *lptr;
        struct pentry *pptr;
	int readwrite;
	int writerprio;
	unsigned long writertime;

        disable(ps);

         if (isbadlock(lck) || (lptr= &locks[lck])->lstate==LFREE) {
                restore(ps);
                return 0;
        }

        else if(lptr->locktype == WRITE || (lptr->locktype == READ && --(lptr->num_readers)<1)){
		kprintf("type %d num readers-1 %d\n",lptr->locktype, lptr->num_readers);
		lptr->locktype = NONE;
		lptr->num_readers = 0;	
		//kprintf("type %d num readers-1 %d\n",lptr->locktype, lptr->num_readers);
		if((isempty(lptr->lrqhead) && nonempty(lptr->lwqhead)) || (proctab[q[lptr->lwqtail].qprev].plckprio > proctab[q[lptr->lrqtail].qprev].plckprio)){
			
			pptr = &proctab[q[lptr->lwqtail].qprev];	
			kprintf("release lock for writer %d\n",q[lptr->lwqtail].qprev );
			lptr->locktype = WRITE;
			lptr->lock_prio = pptr->plckprio;
			lptr->currproc = q[lptr->lwqtail].qprev;
			ready(getlast(lptr->lwqtail), RESCHYES);
			restore(ps);
			return 1;
		}
		else if(isempty(lptr->lrqhead) && isempty(lptr->lwqhead)){
			lptr->locktype = NONE;
			lptr->lock_prio = MININT;
			restore(ps);
			return 1;
		}
		else{
			if(!(isempty(lptr->lrqhead)) && isempty(lptr->lwqhead) ){
				//pptr = &q[q[lptr->lrqtail].qprev]i
				writerprio = MININT;
				writertime = MAXINT;
			}
			else{
				writerprio = proctab[q[lptr->lwqtail].qprev].plckprio;
				writertime = proctab[q[lptr->lwqtail].qprev].lcktime;
			}
			
			int temp;
			temp = q[lptr->lrqtail].qprev;
			while(temp > 0 && temp < NPROC){		
				if(proctab[temp].plckprio > writerprio){
					lptr->locktype = READ;
					lptr->num_readers++;
					ready(getlast(lptr->lwqtail), RESCHYES);
				}

				else if(proctab[temp].plckprio = writerprio && (proctab[temp].lcktime - writertime)<500){
					lptr->locktype = READ;
					lptr->num_readers++;
					ready(getlast(lptr->lwqtail), RESCHYES);
					
				}
				else{
					break;
				}
				
			}
		}

	}	
	
}
