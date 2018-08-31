#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

//Calculates highest waiting priority of the lock. This function does not have any pid as argument. This call is used when a process leaves the queue. This call is used in releasall and kill when process is removed from the wait queue. This function mmodifies the hihgest priority and calls prioinh function to propagate this changes to all the affected locks (transitivity) 

void lmaxprio(int lck){
	int temppid=0;
	struct lentry *lptr = &locks[lck];
	int tempw=0;
	int tempr=0;
	int temp;	
	int max=0;
	int maxpid=0;
	if(nonempty(lptr->lwqhead)){
		temp = q[lptr->lwqtail].qprev;
		while(temp<NPROC){
			tempw = proctab[temp].pprio> proctab[temp].pinh ? proctab[temp].pprio : proctab[temp].pinh;
			if(tempw>max){
				max = tempw;
				maxpid = temp;
			}
			temp = q[temp].qprev;
		}
	}
	if(nonempty(lptr->lrqhead)){
//		kprintf("should get here\n");
		temp = q[lptr->lrqtail].qprev;
		while(temp<NPROC){
			tempw = proctab[temp].pprio> proctab[temp].pinh ? proctab[temp].pprio : proctab[temp].pinh;
			if(tempw>max){
				max = tempw;
				maxpid = temp;
			}
			temp = q[temp].qprev;
		}
		
	}
		
	locks[lck].highest_blckprio = max;	
//	kprintf("called prioing max %d\n",max);
	prioinh(lck, maxpid);
}
