#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include<sem.h>

#define DEFAULT_LOCK_PRIO 20

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }
int mystrncmp(char* des,char* target,int n){
    int i;
    for (i=0;i<n;i++){
        if (target[i] == '.') continue;
        if (des[i] != target[i]) return 1;
    }
    return 0;
}

unsigned long ctr1000;
/*----------------------------------Test 3---------------------------*/
void reader3 (char *msg, int lck, int hint)
{
        int     ret;
	int i=0, j=0, k=0;
        kprintf ("  %s: to acquire lock\n", msg);
        unsigned long t_begin, t_end;
	t_begin = ctr1000;
//	if(hint == 0)
//		lock (lck, READ, DEFAULT_LOCK_PRIO);
//	else
		
		lock (lck, WRITE, DEFAULT_LOCK_PRIO);
	t_end=ctr1000;
	sleep10(2);

	//for(i=0; i<1000000;i++){
	for(i=0; i<100;i++){
		//sleep100(2);
		  for(j=0; j<100; j++){
                if(i%5==0){
                        k++;
                }
                }

		kprintf("%s",msg);
		//	for(j=0; j<1000; j++)
		//sleep100(2);
	}
	//	if(i%1000==0){
	//		kprintf("%s",msg);
	//	}
	//}//i
        releaseall (1, lck);
        kprintf ("lock  %s: acquired lock PRIOIRTY %d\n", msg, getprio(currpid));
        kprintf ("lock  %s: time waited %d\n", msg, t_end-t_begin);
        //releaseall (1, lck);
}

void writer3 (char *msg, int lck)
{
	int i=0, j=0, k=0;
	sleep10(2);	
	for(i=0; i<100;i++){
	//	sleep100(2);
		  for(j=0; j<100; j++){
                if(i%5==0){
                        k++;
                }
                }

		//	for(j=0; j<1000; j++)
		kprintf("%s",msg);
		//sleep100(2);
	}
        //kprintf ("  %s: to acquire lock\n", msg);
        //lock (lck, WRITE, DEFAULT_LOCK_PRIO);
        //kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        //sleep (10);
        kprintf ("  %s: done\n", msg);
        //releaseall (1, lck);
}

/*----------------------------------Test 4---------------------------*/
void reader4 (char *msg, int sem)
{
        int     ret;
	int i=0;
	int j=0;
	int k=0;
	unsigned long t_begin, t_end;
        kprintf ("  %s: to acquire lock\n", msg);
        //lock (lck, READ, DEFAULT_LOCK_PRIO);
        t_begin = ctr1000;
	wait(sem);
	t_end = ctr1000;
	sleep10(2);
	
	for(i=0; i<100;i++){
	//	sleep100(2);
		for(j=0; j<100; j++){
		if(i%5==0){
			k++;
		}
		}
	
		kprintf("%s",msg);
		//for(j=0; j<1000; j++)
		//sleep100(2);
					
	}
	/*for(i=0; i<10;i++){
		//for(j=0; j<1000; j++){
		//if(j==999 && i%1000==0){
		sleep(1);
			kprintf("%s ",msg);
		//}
		//}
	}*/
       signal(sem);
        kprintf ("sem  %s: acquired lock PRIOIRTY %d\n", msg, getprio(currpid));
        kprintf ("sem  %s: time waitedi %u\n", msg, t_end - t_begin);
       // releaseall (1, lck);
       //signal(sem);
}

void writer4 (char *msg, int lck)
{
	int i=0, j=0, k=0;
	sleep10(2);	
	
	for(i=0; i<100;i++){
	//	sleep100(2);
		  for(j=0; j<100; j++){
                if(i%5==0){
                        k++;
                }
                }

		//	for(j=0; j<1000; j++)
		kprintf("%s",msg);
		//		sleep100(2);
					
	}
       // kprintf ("  %s: to acquire lock\n", msg);
        //lock (lck, WRITE, DEFAULT_LOCK_PRIO);
       // kprintf ("  %s: acquired lock, sleep 10s\n", msg);
        //sleep (10);
        kprintf ("  %s: done\n", msg);
        //releaseall (1, lck);
}
void test3 ()
{
        int     lck;
        int     rd1, rd2;
        int     wr1, wr5, wr6, wr3, wr4;

        kprintf("\nTest 3: test the basic priority inheritence\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 3 failed");

        rd1 = create(reader3, 2000, 20, "reader3", 2, "A", lck, 0);
        rd2 = create(reader3, 2000, 50, "reader3", 2, "B", lck, 1);
        wr1 = create(writer3, 2000, 25, "writer3", 2, "C", lck);
        wr5 = create(writer3, 2000, 25, "writer3", 2, "C", lck);
        wr6 = create(writer3, 2000, 25, "writer3", 2, "C", lck);
	resume(wr1);
	resume(rd1);
	//sleep(1);	
	resume(wr5);
	//sleep(1);
	resume(wr6);
	resume(rd2);	
	

	sleep(20);
	int rd4, rd5, wr2;	
	int s = screate(1);
        rd4 = create(reader4, 2000, 20, "reader3", 2, "C", s);
        rd5 = create(reader4, 2000, 50, "reader3", 2, "D", s);
        wr2 = create(writer4, 2000, 25, "writer3", 2, "E", s);
        wr3 = create(writer4, 2000, 25, "writer3", 2, "E", s);
        wr4 = create(writer4, 2000, 25, "writer3", 2, "E", s);
        //kprintf("-start writer, then sleep 1s. lock granted to write (prio 20)\n");
     //   resume(wr2);
        //sleep (1);
        resume(wr2);
	resume(rd4);
	//sleep(1);
        //kprintf("-start reader A, then sleep 1s. reader A(prio 25) blocked on the lock\n");
        resume(wr3);
        resume(wr4);
        resume(rd5);
	
        //sleep (1);
	//assert (getprio(wr1) == 25, "Test 3 failed");

        //kprintf("-start reader B, then sleep 1s. reader B(prio 30) blocked on the lock\n");
        //resume (rd2);
	//sleep (1);
	//assert (getprio(wr1) == 30, "Test 3 failed");
	
//	kprintf("-kill reader B, then sleep 1s\n");
//	kill (rd2);
///	sleep (1);
//	assert (getprio(wr1) == 25, "Test 3 failed");

//	kprintf("-kill reader A, then sleep 1s\n");
//	kill (rd1);
//	sleep(1);
//	assert(getprio(wr1) == 20, "Test 3 failed");

        sleep (10);
        kprintf ("Test 3 OK\n");
}

int main( )
{
        /* These test cases are only used for test purpose.
 *          * The provided results do not guarantee your correctness.
 *                   * You need to read the PA2 instruction carefully.
 *                            */
//	test1();
//	test2();
	test3();

        /* The hook to shutdown QEMU for process-like execution of XINU.
 *          * This API call exists the QEMU process.
 *                   */
        shutdown();
}





