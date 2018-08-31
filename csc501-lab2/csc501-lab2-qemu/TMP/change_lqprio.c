#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>


//Change the highest waiting process priority of the lock lid if pid has a priority higher than that

int change_lqprio(int lck, int pid){
	int temp;
//	kprintf("change prio pid %d pprio %d  pinh %d highest %d\n",pid, proctab[pid].pprio, proctab[pid].pinh, locks[lck].highest_blckprio);
	if(proctab[pid].pinh>0){
		temp = proctab[pid].pinh;
	}
	else{
		temp = proctab[pid].pprio;
	}

	if(temp >= locks[lck].highest_blckprio){
		locks[lck].highest_blckprio=temp;
		return 1;
	}
	else{
		return 0;
	}
	
}

