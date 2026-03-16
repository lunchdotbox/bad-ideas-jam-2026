#ifndef ENGINE_UTILITIES_TIME_H
#define ENGINE_UTILITIES_TIME_H

#include "integers.h"
#include "inline.h"

#include <memory.h>
#include <sys/time.h>

INLINE u64 milliseconds() {
    struct timeval time;
    gettimeofday(&time, NULL);
    u64 s1 = (u64)(time.tv_sec) * 1000;
    u64 s2 = time.tv_usec / 1000;
    return s1 + s2;
}

INLINE u64 stoms(double s) {
    return s * 1000;
}

#endif
