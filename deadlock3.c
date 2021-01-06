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


#define DEVICE "/dev/a5"

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

int thread_id = 0;

#define MODE1 1
#define MODE2 2


void *scenario(void *ptr)
{
  printf("Current Thread %d \n", thread_id++);              // Intention of ++ here
  int file_descriptor = open(DEVICE, O_RDWR);
  sleep(1);

  if(file_descriptor == -1)
  {
    printf(" Device %s is not avaliable to open", DEVICE);
    exit(-1);
  }

  int ctrl = ioctl(file_descriptor, E2_IOCMODE1, 1);
  close(file_descriptor);
  pthread_exit(NULL);

}


int main( int argc, char* argv[])
{
  pthread_t thread_id[2];
  long k;

  int file_descriptor = open(DEVICE, O_RDWR);
  int ctrl = ioctl(file_descriptor, E2_IOCMODE2, 2);
  sleep(1);

  pthread_create(&(thread_id[0]), NULL, &scenario, NULL);
  sleep(5);
  pthread_create(&(thread_id[1]), NULL, &scenario, NULL);

  for(k = 0; k<2; k++)
  {
    pthread_join(thread_id[k], NULL);
  }

  return 0;
}