#ifndef _CHECK_H
#define _CHECK_H 1

#include <stdio.h>
#include <stdlib.h>

#define ABORT(FILE, LINE) do { \
    fprintf(stderr, "Abort at file %s : line %d\n", FILE, LINE); \
    abort(); \
} while(0)

#define check(COND) do { \
    if (!(COND)) { \
        ABORT(__FILE__, __LINE__); \
    } \
} while(0)

#endif