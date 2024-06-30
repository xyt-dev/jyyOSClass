#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include "thread.h"

sem_t empty, fill;

#define INIT_SEM(X, Y) sem_init(&X, 0, (Y))
#define P(X) sem_wait(&X)
#define V(X) sem_post(&X)

void producer() {
  int count = 1;
  while (1) {
    usleep(10000);
    if (count % 60 == 0)
      sleep(1);
    P(empty);
      printf("(");
      fflush(stdout); // Note
    V(fill);
    count ++;
  }
}

void consumer() {
  int count = 1;
  while (1) {
    usleep(10000);
    if ((count + 30) % 60 == 0)
      sleep(1);
    P(fill);
      printf(")");
      fflush(stdout);
    V(empty);
    count ++;
  }
}

int main() {
  INIT_SEM(empty, 10); 
  INIT_SEM(fill, 0);
  create((void *)producer);
  create((void *)consumer);
  join();
  return 0;
}



