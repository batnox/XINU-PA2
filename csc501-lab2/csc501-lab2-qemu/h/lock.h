#ifndef _LOCK_H_
#define _LOCK_H_

#define NLOCKS 50

#define LFREE '\01'
#define LUSED '\02'

#define NONE -1
#define READ 0
#define WRITE 1
struct lentry {
	char lstate;
	int lcnt;
	int lwqhead;
	int lwqtail;
	int lrqhead;
	int lrqtail;
	int locktype;
	int lock_prio;
	int lock_holders[NPROC];
	int highest_blckprio;
	int num_readers;	
	int currproc;
	//int proc_related_h;
	//int proc_related_t;
};

extern struct lentry locks[];
extern int nextlock;
#define isbadlock(l)	(l<0 || l>=NLOCKS)
#define isnegative(n)	(n<0)
SYSCALL lock(int lck, int ltype, int lpr);
SYSCALL releaseall(int, long, ...);
SYSCALL lcreate(void);
SYSCALL ldelete(int sem);
void linit(void);
int change_lqprio(int lck, int prio);
void inverse_prio(int lck, int prio);
void modifyready(int pid, int prio);

#endif
