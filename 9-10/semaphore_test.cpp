#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include "thread.h"
#include "semaphore.h"

// sem_t sem;
// sem_t sem_join;

// void T_worker(int id) {
//   sem_wait(&sem);
//   for (int j = 0; j < 5; j ++) {
//     printf("id = %d.\n", id);
//     usleep(1000000);
//   }
//   sem_post(&sem);
  // sem_post(&sem_join);
// }

// int main() {
//   basic_test();
//   sem_init(&sem, 0, 5);
//   // sem.__size[0] = 3;
//   sem_init(&sem_join, 0, 0);

//   for (int j = 0; j < 10; j ++)
//     create((void *)T_worker);
//   sem_wait(&sem);
//   for (int j = 0; j < 10; j ++)
//     sem_wait(&sem_join);
//   return 0;
// }

Semaphore sem(5);
Semaphore sem_join(0);

void T_worker(int id) {
  sem.wait();
  printf("id = %d sem = %d\n", id, sem.getSemValue());
  usleep(1000000);
  sem.post();
  sem_join.post();
}

void T_worker1() {
  printf("wait.\n");
  sem.wait();
  sem_join.post();
}

void T_worker2() {
  sem.post();
  printf("post. sem = %d\n", sem.getSemValue());
  sem_join.post();
}

int main() {
  basic_test();
  // create((void *)T_worker1);
  // usleep(1000);
  // create((void *)T_worker2);
  // join();
  for (int j = 0; j < 50; j ++) {
    create((void *)T_worker);
  }
  for (int j = 0; j < 50; j ++) {
    sem_join.wait();
  }
  return 0;
}
