#include <conf.h>
#include <kernel.h>
#include <lock.h>

struct lentry locks[NLOCKS];

void linit(void)
{
	int i=0;
	struct lentry *lptr;
	for(i=0; i<NLOCKS; i++){
		lptr = &locks[i];
		lptr->lstate = LFREE;
		lptr->lwqtail = 1 + (lptr->lwqhead = newqueue());
		lptr->lrqtail = 1 + (lptr->lrqhead = newqueue());
		lptr->lock_prio = 0;
	
	}

}
