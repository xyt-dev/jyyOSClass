#include "thread.h"

volatile long sum = 0;
volatile int A = 0, B = 0;
volatile int turn = 0;
const long N = 10000000;

void TA() {
    for (long i = 0; i < N; i ++) {
        A = 1;
        __sync_synchronize(); // memory barrier 防止处理器指令重排，同时保证该处理器之前的所有写操作对其他处理器可见(写回内存之前保证其它处理器缓存无效)
        turn = 1;
        __sync_synchronize();
        while (B == 1 && turn != 0);
        __sync_synchronize();
        sum++;
        __sync_synchronize();
        A = 0;
    }
}


void TB() {
    for (long i = 0; i < N; i ++) {
        B = 1;
        __sync_synchronize();
        turn = 0;
        __sync_synchronize();
        while (A == 1 && turn != 1);
        __sync_synchronize();
        sum++;
        __sync_synchronize();
        B = 0;
    }
}

int main() {
    create((void *)TA);
    create((void *)TB);
    join();
    printf("sum = %ld\n", sum);
    return 0;
}