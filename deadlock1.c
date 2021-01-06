#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#include <linux/ioctl.h>

// # Scenario - > If there are 2 threads and one is blocked in open() and the other is blocked in ioctl()
// - The driver is in Mode 1 and if both the threads try to call open function.
// - Thread 1 acquires lock sem1-> Increments count1 to 1 -> Acquires lock sem2 -> Releases lock sem 1
// - Thread 2 acquires lock sem1-> Increments count1 to 2 -> Tries to acquire lock sem2 but as it is already locked, T2 gets blocked until it gets unlocked
// -  Thread 1 calls e2_ioctl() -> Aquires sem 1 -> Enter while loop as count1 >1 -> Release sem 1 -> T1 gets blocked in wait_event_interruptible() until count1 becomes 1
// - Both T1 and T2 will be blocked for Mode 1 and 2

#define DEVICE "/dev/a5"              // Try changing it

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

#define MODE1 1 
#define MODE2 2
int tid =0;

void *funct(void *th){

	int rc, fd;
	printf("Thread %d\n", tid++);
	fd = open(DEVICE, O_RDWR);
	sleep(0.6);	
	if(fd == -1) {
		printf("File %s does not exist. Exiting!!\n", DEVICE);
		exit(-1);
	}
	rc = ioctl(fd, E2_IOCMODE2, 1);
	close(fd);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	long i, j;
	pthread_t tid[2];

	for(i = 0; i<2; i++) 
		pthread_create(&tid[i], NULL, funct, (void *) i);
	for(j = 0; j<2; j++)		
		pthread_join(tid[j],NULL);

	return 0;
}