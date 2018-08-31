/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate(void)
{
	STATWORD ps;    
	int	lock;

	disable(ps);
	if ((lock=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	//locks[lock].lcreate = count;
	locks[lock].num_readers = 0;
	locks[lock].locktype = NONE;
	int i=0;
	for(i=0; i<NPROC; i++){
		locks[lock].lock_holders[i]=-1;
	//	locks[lock].proc_related[i] = 0;
	}

	locks[lock].highest_blckprio=-1;
	locks[lock].currproc=0;
	/* sqhead and sqtail were initialized at system startup */
	restore(ps);
	return(lock);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int	lock;
	int	i;

	for (i=0 ; i<NLOCKS ; i++) {
		lock=nextlock--;
		if (nextlock < 0)
			nextlock = NLOCKS-1;
		if (locks[lock].lstate==LFREE) {
			locks[lock].lstate = LUSED;
			return(lock);
		}
	}
	return(SYSERR);
}
