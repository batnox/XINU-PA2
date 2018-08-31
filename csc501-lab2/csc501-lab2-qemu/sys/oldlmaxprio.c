#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

void lmaxprio(int lck){
	int temppid=0;
	struct lentry *lptr = locks[lck];
	int tempw=0;
	int tempr=0;
	
	if(nonempty(lptr->lwqhead)){
		while()
		tempw = proctab[q[lptr->lwqtail].qprev].pprio> proctab[q[lptr->lwqtail].qprev].pinhi ? proctab[q[lptr->lwqtail].qprev].pprio : proctab[q[lptr->lwqtail].qprev].pinh;

	}
	if(nonempty(lptr->lrqhead)){
		tempr = q[lptr->lrqtail].qprev;
		tempr= proctab[q[lptr->lrqtail].qprev].pprio> proctab[q[lptr->lrqtail].qprev].pinhi ? proctab[q[lptr->lrqtail].qprev].pprio : proctab[q[lptr->lrqtail].qprev].pinh;
	}
	if(tempr == 0 && tempw == 0){
		temppid=0;
	}
	else if(tempr>tempw){
		temppid = q[lptr->lrqtail].qprev; 
	}
	else{
		temppid = q[lptr->lwqtail].qprev;
	}
	locks[lck].highest_blockprio = tempr>tempw ? tempr : tempw;	
	prioinh(lck, temppid);
}
