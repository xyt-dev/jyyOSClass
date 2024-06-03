#include "thread.h"

__thread char *base, *cur;
__thread int id;

__attribute__((noinline)) void set_cur(void *ptr) {cur = (char *)ptr;}
__attribute__((noinline)) char *get_cur() {return cur;}

void stackOverFlow(int n) {
    set_cur(&n);
    if (n % 1024 == 0) {
        int sz = base - cur;
        printf("Stack size of T%d >= %d KB\n", id, sz / 1024);
    }
    stackOverFlow(n + 1);
}

void Tprobe(int tid) {
    id = tid;
    base = (char *)&tid;
    stackOverFlow(0);
}

int main() {
    for (int i = 1; i <= 3; i ++) {
        create((void *)Tprobe);
    }
    // create((void *)Tprobe);
    join();
    return 0;
}