#include "mutex.h"
#include "thread.h"
#include <ctime>

Mutex mutex;

void basicTest() {
  Mutex mutex1;
  mutex1.lock();
  mutex1.unlock();
  mutex1.lock();
  mutex1.unlock();
  mutex1.lock();
  {
    int a = 0;
    int b = 1;
    b = xchg(&a, b);
    assert(a == 1 && b == 0);
  }
  int c = 2;
  c = atomic_dec(&c);
  assert(c == 2);
  atomic_dec(&c);
  c = atomic_dec(&c);
  assert(c == 1);
}

volatile int sum = 0;

void T() {
  for (int j = 0; j < 100000000; j++) {
    mutex.lock();
    sum++;
    mutex.unlock();
  }
}

int main() {
  basicTest();
  struct timespec start, end;
  double cpu_time_used;

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (int j = 0; j < 3; j++)
    create((void *)T);
  join();
  std::cout << "Sum: " << sum << std::endl;

  clock_gettime(CLOCK_MONOTONIC, &end);
  cpu_time_used =
      (end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
  std::cout << "runtime: " << cpu_time_used << "s." << std::endl;
  return 0;
}
