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

// # Two threads created intially with opposite modes and tries to change their modes by ioctl()
// - Two threads created such that T1 is in Mode2 and T2 is in Mode 1
// - Both the threads try to chnage their mode
// - T2 will change its mode to Mode 2 before T1 changes its mode. To reproduce this scenario, 
//   we need to add sleep statement in line 165 of the given code.
// - There will be a time when both the threads wil be in Mode 2 and if thread 1 tries to change its mode to 1, 
//   then it will be blocked as the count2 > 1
// -Both threads will wait for each other to terminate and reduce the count2, which will cause deadlock.


#define DEVICE "/dev/a5"

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

int thread_id = 0;

#define MODE1 1 
#define MODE2 2


void *First_thread()
{
  sleep(2);
  int file_descriptor = open(DEVICE, O_RDWR);

  if(file_descriptor == -1)
  {
    printf(" Device %s is not avaliable to open", DEVICE);
    exit(-1);
  }

  ioctl(file_descriptor, E2_IOCMODE2, 0);
  ioctl(file_descriptor, E2_IOCMODE1, 0);

  close(file_descriptor);
printf("thread_id 1 completed its work %s\n", DEVICE);
  pthread_exit(NULL);

}

void *Second_thread()
{
  //sleep(2);
  int file_descriptor = open(DEVICE, O_RDWR);

  if(file_descriptor == -1)
  {
    printf(" Device %s is not avaliable to open", DEVICE);
    exit(-1);
  }

  ioctl(file_descriptor, E2_IOCMODE2, 0);

  sleep(5);

  ioctl(file_descriptor, E2_IOCMODE1, 0);

  close(file_descriptor);
printf("thread_id 2 completed its work %s\n", DEVICE);
  pthread_exit(NULL);

}

int main()
{
  pthread_t thread_id[2];
  pthread_create(&(thread_id[0]), NULL, First_thread, NULL);
  pthread_create(&(thread_id[1]), NULL, Second_thread, NULL);
  pthread_exit(NULL);
  printf("process finished\n");
  return 0;
}
