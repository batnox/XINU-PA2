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
/*--------------------------------Test 1--------------------------------*/
 
void reader1 (char *msg, int lck)
{
	lock (lck, READ, DEFAULT_LOCK_PRIO);
	kprintf ("  %s: acquired lock, sleep 2s\n", msg);
	sleep (2);
	kprintf ("  %s: to release lock\n", msg);
	releaseall (1, lck);
}

void test1 ()
{
	int	lck;
	int	pid1;
	int	pid2;

	kprintf("\nTest 1: readers can share the rwlock\n");
	lck  = lcreate ();
	assert (lck != SYSERR, "Test 1 failed");

	pid1 = create(reader1, 2000, 20, "reader a", 2, "reader a", lck);
	pid2 = create(reader1, 2000, 20, "reader b", 2, "reader b", lck);

	resume(pid1);
	resume(pid2);
	
	sleep (5);
	ldelete (lck);
	kprintf ("Test 1 ok\n");
}

/*----------------------------------Test 2---------------------------*/
char output2[10];
int count2;
void reader2 (char msg, int lck, int lprio)
{
        int     ret;

        kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, READ, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
        kprintf ("  %c: to release lock\n", msg);
	releaseall (1, lck);
}

void writer2 (char msg, int lck, int lprio)
{
	kprintf ("  %c: to acquire lock\n", msg);
        lock (lck, WRITE, lprio);
        output2[count2++]=msg;
        kprintf ("  %c: acquired lock, sleep 3s\n", msg);
        sleep (3);
        output2[count2++]=msg;
        kprintf ("  %c: to release lock\n", msg);
        releaseall (1, lck);
}

void test2 ()
{
        count2 = 0;
        int     lck;
        int     rd1, rd2, rd3, rd4;
        int     wr1;

        kprintf("\nTest 2: wait on locks with priority. Expected order of"
		" lock acquisition is: reader A, reader B, reader D, writer C & reader E\n");
        lck  = lcreate ();
        assert (lck != SYSERR, "Test 2 failed");

	rd1 = create(reader2, 2000, 20, "reader2", 3, 'A', lck, 20);
	rd2 = create(reader2, 2000, 20, "reader2", 3, 'B', lck, 30);
	rd3 = create(reader2, 2000, 20, "reader2", 3, 'D', lck, 25);
	rd4 = create(reader2, 2000, 20, "reader2", 3, 'E', lck, 20);
        wr1 = create(writer2, 2000, 20, "writer2", 3, 'C', lck, 25);
	
        kprintf("-start reader A, then sleep 1s. lock granted to reader A\n");
        resume(rd1);
        sleep (1);

        kprintf("-start writer C, then sleep 1s. writer waits for the lock\n");
        resume(wr1);
        sleep10 (1);


        kprintf("-start reader B, D, E. reader B is granted lock.\n");
        resume (rd2);
	resume (rd3);
	resume (rd4);


        sleep (15);
        kprintf("output=%s\n", output2);
        assert(mystrncmp(output2,"ABABDDCCEE",10)==0,"Test 2 FAILED\n");
        kprintf ("Test 2 OK\n");
}

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
	//sleep(2);

	//for(i=0; i<1000000;i++){
	for(i=0; i<100;i++){
		sleep100(2);
		  for(j=0; j<100; j++){
                if(i%5==0){
                        k++;
                }
                }

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
	//sleep(3);	
	for(i=0; i<100;i++){
		sleep100(2);
		  for(j=0; j<100; j++){
                if(i%5==0){
                        k++;
                }
                }

		//	for(j=0; j<1000; j++)
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
	//sleep(2);
	
	for(i=0; i<100;i++){
		sleep100(2);
		for(j=0; j<100; j++){
		if(i%5==0){
			k++;
		}
		}
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
//	sleep(3);	
	
	for(i=0; i<100;i++){
		sleep100(2);
		  for(j=0; j<100; j++){
                if(i%5==0){
                        k++;
                }
                }

		//	for(j=0; j<1000; j++)
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
        int     wr1;

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





