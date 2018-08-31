#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
/*
prioinh functiion gets the lock and the new / updated pid in argument. It passes them to chnge_pqprio to modify the highest lock priority if required. If there is any change in it, we check for all the pocesses holding that lock and then other locks that are affected by these processes. This is an iterative process. Each time there is a change in the priority of a proces and it has a new lock that is blocking it, we add the lock to a queue. Each iteration we dequeue a lock and process it (similar to BFS using a queue) 
*/

void modifyready(int pid, int prio){
	int temp;
	temp = q[rdytail].qprev;
	while(temp<NPROC){
		if(temp == pid){
			q[q[temp].qprev].qnext = q[temp].qnext;
			q[q[temp].qnext].qprev = q[temp].qprev;
			insert(temp, rdyhead,prio);
			break;
			
		}
		temp = q[temp].qprev;
	}
}

void prioinh(int lck, int pid){
	//int currqhead;
  	//int currqtail = 1 + (currqhead = newqueue());
	//locks[lck] = enqueue(locks[lck], currqtail);
	int currlocks[NLOCKS];
	int corr_pid[NLOCKS];
	int head=0;
	int tail=0;
	int i=0;
	int j=0;
	int visited[NLOCKS];
	for(i=0; i<NLOCKS; i++){
		currlocks[i]=-1;
		corr_pid[i]=-1;
		visited[i] = 0;
	}
	currlocks[head] = lck;
	corr_pid[head] = pid;
	tail++;
	visited[lck] = 1;
	while(currlocks[head]!=-1){
		
		
		if(change_lqprio(currlocks[head], corr_pid[head])==1){

			for(i=0; i<NPROC; i++){
				if(locks[lck].lock_holders[i] == 1){
					//kprintf("LOCK HOLDER %d\n",i);
					//int temp_prio = proctab[i].pprio>proctab[i].pinh ? proctab[i].pprio : proctab[i].pinh;
					//if(proctab[i].pprio<locks[currlocks[head]].highest_blckprio){
						//proctab[i].pinh = locks[currlocks[head]].highest_blockprio; 
						//dequeue(i);
						//insert(pid, rdyhead,pptr->pinh);	
						int max=locks[currlocks[head]].highest_blckprio;
						//kprintf("HIGHEST BLOCKPRIO %d\n", max);
						for(j=0; j<NLOCKS; j++){
							if(proctab[i].locks_held[j]==1){
								//currlocks[tail]=j;
								//tail++;
								if(locks[j].highest_blckprio > max){
									max = locks[j].highest_blckprio;
								}
							}
						}
						proctab[i].pinh = max;
						//kprintf("before modify %d\n",proctab[i].pinh);
						if(proctab[i].pprio<max){
                                               		proctab[i].pinh = max;
                                                	modifyready(i,proctab[i].pinh);
                                        	}

						else{
							proctab[i].pinh = -1;
                                                	modifyready(i,proctab[i].pprio);
						}
		//dequeue(i);
						//insert(i, rdyhead, proctab[i].pinh);
						//kprintf("before visited\n");
						if(visited[proctab[i].lock_blocked] != 1 && proctab[i].lock_blocked!=-1){
							currlocks[tail]=proctab[i].lock_blocked;
							corr_pid[tail] = i;
							visited[proctab[i].lock_blocked]=1;
							tail++;
						}
					//}

					/*else if(proctab[i].pinh != -1){
						proctab[i].pinh = -1;
						dequeue(i);
						insert(i, rdyhead, proctab[i].pprio);
					}*/
						
					
				}
								
			}
		}

		head++;

	}
}
