#ifndef NU_TIME_IMPL_H
#define NU_TIME_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_PLATFORM_WINDOWS
#define MS_PER_SEC 1000ULL // MS = milliseconds
#define US_PER_MS  1000ULL // US = microseconds
#define HNS_PER_US 10ULL   // HNS = hundred-nanoseconds (e.g., 1 hns = 100 ns)
#define NS_PER_US  1000ULL

#define HNS_PER_SEC (MS_PER_SEC * US_PER_MS * HNS_PER_US)
#define NS_PER_HNS  (100ULL) // NS = nanoseconds
#define NS_PER_SEC  (MS_PER_SEC * US_PER_MS * NS_PER_US)

int
clock_gettime_monotonic (struct timespec *tv)
{
    static LARGE_INTEGER ticksPerSec;
    LARGE_INTEGER        ticks;

    if (!ticksPerSec.QuadPart)
    {
        QueryPerformanceFrequency(&ticksPerSec);
        if (!ticksPerSec.QuadPart)
        {
            errno = ENOTSUP;
            return -1;
        }
    }

    QueryPerformanceCounter(&ticks);

    tv->tv_sec  = (long)(ticks.QuadPart / ticksPerSec.QuadPart);
    tv->tv_nsec = (long)(((ticks.QuadPart % ticksPerSec.QuadPart) * NS_PER_SEC)
                         / ticksPerSec.QuadPart);

    return 0;
}
#endif

nu_time_t
nu_time (void)
{
    nu_time_t t;
    t.value = time(NULL);
    return t;
}
nu_u32_t
nu_time_hours (const nu_time_t *time)
{
    struct tm *t = localtime(&time->value);
    return t->tm_hour;
}
nu_u32_t
nu_time_minutes (const nu_time_t *time)
{
    struct tm *t = localtime(&time->value);
    return t->tm_min;
}
nu_u32_t
nu_time_seconds (const nu_time_t *time)
{
    struct tm *t = localtime(&time->value);
    return t->tm_sec;
}

static inline void
timespec_diff (struct timespec *start,
               struct timespec *stop,
               struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0)
    {
        result->tv_sec  = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    }
    else
    {
        result->tv_sec  = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
    return;
}

void
nu_timer_reset (nu_timer_t *timer)
{
#ifdef NU_PLATFORM_WINDOWS
    clock_gettime_monotonic(&timer->start);
#else
    clock_gettime(CLOCK_MONOTONIC, &timer->start);
#endif
}
nu_f32_t
nu_timer_elapsed (nu_timer_t *timer)
{
    struct timespec end;
#ifdef NU_PLATFORM_WINDOWS
    clock_gettime_monotonic(&end);
#else
    clock_gettime(CLOCK_MONOTONIC, &end);
#endif

    struct timespec diff;
    timespec_diff(&timer->start, &end, &diff);

    return (nu_f64_t)diff.tv_nsec / 1.0e6;
}

#endif
