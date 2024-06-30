#include <am.h>
#include <stdarg.h>
#include <stdio.h>

void print(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    while (s) {
        for (; *s; s ++) putch(*s);
        s = va_arg(ap, const char *);
    }
    va_end(ap);
}

void putInt(int num) {
    putch('0');
    putch('x');
    for (int i = 31; i >= 0; i --) {
        putch(((num >> i) & 1) + '0');
    }
    putch('\n');
}


int main() {
    putInt(666);
    return 0;
}
