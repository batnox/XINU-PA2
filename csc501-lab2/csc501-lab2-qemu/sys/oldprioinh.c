#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>


void prioinh(int lck, int pid){
	//int currqhead;
  	//int currqtail = 1 + (currqhead = newqueue());
	//locks[lck] = enqueue(locks[lck], currqtail);
	int currlocks[NLOCKS];
	int head=0;
	int tail=0;
	int i=0;
	int j=0;
	int visited[NLOCKS];
	for(i=0; i<NLOCKS; i++){
		currlocks[i]=-1;
		visited[i] = 0;
	}
	currlocks[head] = lck;
	tail++;
	visited[lck] = 1;
	while(currlocks[head]!=-1){
		
		
		if(change_lqprio(currlocks[head], pid)){

			for(i=0; i<NPROCS; i++){
				if(lock_holders[i] == 1){
					int temp_prio = proctab[i].pprio>proctab[i].pinh ? proctab[i].pprio : proctab[i].pinh;
					if(temp_prio<locks[currlocks[head]].highest_blockprio){
						proctab[i].pinh = locks[currlocks[head]].highest_blockprio; 
						dequeue(i);
						insert(pid, rdyhead,pptr->pinh);	
					
						for(j=0; j<NLOCKS; j++){
							if(proctab[i].locks_held[j] == 1 && visited[j] == 0){
								currlocks[tail]=j;
								tail++;
							}
						}
					}
						
					
				}
								
			}
		}

		head++;

	}
}
