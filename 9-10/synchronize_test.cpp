#include "mutex.h"
#include "thread.h"
#include <unistd.h>
#include <semaphore.h>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

const int N = 7;

struct Edge {
  int from, to;
  Mutex mutex;
} edges[] = {
  {1, 2, Mutex()},
  {2, 3, Mutex()},
  {2, 4, Mutex()},
  {2, 5, Mutex()},
  {3, 6, Mutex()},
  {3, 7, Mutex()},
  {-1, -1, Mutex()},
};

void T_worker(int id) {
  for (int j = 0; edges[j].from != -1; j ++) {
    if (edges[j].to == id) edges[j].mutex.lock();
  }
  sleep(1);
  printf("worker %d Ok.\n", id);
  for (int j = 0; edges[j].from != -1; j ++) {
    if (edges[j].from == id) edges[j].mutex.unlock();
  }
}

int main() {
  for (int i = 0; edges[i].from != -1; i ++) {
    edges[i].mutex.lock();
  }
  for (int i = 0; i < N; i ++) {
    create((void *)T_worker);
  }
  join();
  return 0;
}
