#include <time.h>
#include <sys/time.h>
#include "measure_time.h"

long long get_current_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LL + tv.tv_usec;
}

