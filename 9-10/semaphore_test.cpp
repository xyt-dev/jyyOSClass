#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include "thread.h"

sem_t sem;
sem_t sem_join;

void T_worker(int id) {
  sem_wait(&sem);
  for (int j = 0; j < 5; j ++) {
    printf("id = %d.\n", id);
    usleep(1000000);
  }
  sem_post(&sem);
  sem_post(&sem_join);
}

int main() {
  sem_init(&sem, 0, 5);
  // sem.__size[0] = 3;
  sem_init(&sem_join, 0, 0);

  for (int j = 0; j < 10; j ++)
    create((void *)T_worker);
  sem_wait(&sem);
  for (int j = 0; j < 10; j ++)
    sem_wait(&sem_join);
  return 0;
}
