#include "thread.h"
#include <cassert>
#include <iostream>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <unistd.h>

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
  int dec = -1;
  asm volatile("lock xaddl %0, %1" : "+r"(dec), "+m"(*val) : : "memory");

  return dec;
}

class Mutex {
private:
  int val;
  int futex_wait(int *addr, int val) {
    // SYS_futex: 系统调用号
    // uaddr: 目标变量地址
    // futex_op: 操作类型
    // val: 等待的值
    // timeout: 备用，超时时间
    // uaddr2: 备用地址
    // val3: 备用，在futex_wait中为0
    return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
  }
  int futex_wake(int *addr, int val) {
    // SYS_futex: 系统调用号
    // uaddr: 目标变量地址
    // futex_op: 操作类型
    // val: 指定要唤醒的线程数量，最多为val个线程
    // timeout: 备用，超时时间
    // uaddr2: 备用地址
    // val3: 备用，在futex_wake中为0
    return syscall(SYS_futex, addr, FUTEX_WAKE, val, NULL, NULL, 0);
  }

public:
  Mutex() : val(0) {}
  int lock() {
    int c;
    if ((c = cmpxchg(&val, 0, 1)) != 0) {
      if (c != 2) {
        c = xchg(&val, 2);
      }
      while (c != 0) {
        futex_wait(&val, 2);
        c = xchg(&val, 2);
      }
    }
    return c;
  }
  void unlock() {
    if (atomic_dec(&val) != 1) { // 返回 atomic_dec 之前的值
      val = 0;
      futex_wake(&val, 1);
    }
  }
  int spin_lock() {
    int c;
    while ((c = cmpxchg(&val, 0, 1)) != 0) {
    }
    return c;
  }
  void spin_unlock() {
    asm volatile("" ::: "memory");
    val = 0;
  }
};
