#include <stdio.h>
#include <unistd.h>

#define REGS_FOREACH(_) _(X) _(Y)
#define OUTS_FOREACH(_) _(A) _(B) _(C) _(D) _(E) _(F) _(G)
#define RUN_LOGIC   X1 = (!X && Y) || (X && !Y); \
                    Y1 = (!X && !Y) || (X && !Y); \
                    A = (!X && !Y) || (X && !Y) || (X && Y); \
                    B = 1; \
                    C = (!X && !Y) || (!X && Y) || (X && Y); \
                    D = (!X && !Y) || (X && !Y) || (X && Y); \
                    E = (!X && !Y) || (X && !Y); \
                    F = (!X && !Y); \
                    G = (X && !Y) || (X && Y);

#define DEFINE(X) static int X, X##1;
#define UPDATE(X) X = X##1;
#define PRINT(X) printf(#X " = %d; ", X);

int main() {
    REGS_FOREACH(DEFINE)
    OUTS_FOREACH(DEFINE)
    while (true) {
        RUN_LOGIC
        OUTS_FOREACH(PRINT)
        REGS_FOREACH(UPDATE)
        putchar('\n');
        fflush(stdout);
        sleep(1);
    }
    return 0;
}