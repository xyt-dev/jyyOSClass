#include <cassert>
#include <cstddef>
#include <cstdio>
#include <unistd.h>
#include <syscall.h>
#include <linux/futex.h>
#include <pthread.h>

int cmpxchg(int *valPtr, int oldVal, int newVal) {
  int ret;
  asm volatile("lock cmpxchg %2, %1"
              : "=a"(ret), "+m"(*valPtr)
              : "r"(newVal), "a"(oldVal)
              : "memory");
  return ret;
}

int xchg(int *oldValPtr, int newVal) {
  int ret;
  asm volatile("xchgl %0, %1"
              : "+m"(*oldValPtr), "=r"(ret)
              : "1"(newVal)
              : "memory");
  return ret;
}

int atomic_dec(int *val) {
  int ret = -1;
  asm volatile("lock xaddl %0, %1"
              : "+r"(ret), "+m"(*val)
              :
              : "memory");
  return ret;
}

int atomic_inc(int *val) {
  int ret = 1;
  asm volatile("lock xaddl %0, %1"
              : "+r"(ret), "+m"(*val)
              :
              : "memory");
  return ret;
}

bool atomic_dec_if_positive(int *val) {
  int c;
  do {
    c = *val;
    if (c <= 0) return false; // 确保 val >= 0 同时实现原子的[判断+赋值] (实现方法：保证不被打断/被打断就重做)
  } while(cmpxchg(val, c, c - 1) != c);
  return true;
}

void basic_test() {
  int a = 0;
  assert(cmpxchg(&a, 0, 1) == 0);
  assert(cmpxchg(&a, 0, 2) == 1);
  assert(cmpxchg(&a, 1, 2) == 1);
  assert(xchg(&a, -1) == 2);
  assert(atomic_inc(&a) == -1);
  assert(atomic_inc(&a) == 0);
  assert(atomic_inc(&a) == 1);
  assert(atomic_dec(&a) == 2);
  assert(atomic_dec(&a) == 1);
}

class Semaphore {
private:
  int sem_val;
  int futex_wait(int *addr, int val) {
    return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, NULL);
  }
  int futex_wake(int *addr, int val) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, val, NULL, NULL, NULL);
  }
public:
  Semaphore(int val) : sem_val(val) {}
  void wait() {
    while (1) {
      if (atomic_dec_if_positive(&sem_val)) break;
      futex_wait(&sem_val, 0);
    }
  }
  void post() {
    atomic_inc(&sem_val);
    futex_wake(&sem_val, 1);
  }
  int getSemValue() {
    return sem_val;
  }
};



