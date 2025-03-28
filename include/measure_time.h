#ifndef _H_MEASURE_TIME
#define _H_MEASURE_TIME

#include <time.h>
#include <sys/time.h>
long long get_current_time_us();

#define MEASURE_TIME_START() long long start_time = get_current_time_us()
#define MEASURE_TIME_RESTART() start_time = get_current_time_us()
#define MEASURE_TIME_END(label) do { \
    long long end_time = get_current_time_us(); \
    printf("%s: %.10lf s\n", label, (end_time - start_time) / 1000000000.0); \
} while (0)

#endif
