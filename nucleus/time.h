#ifndef NU_TIME_H
#define NU_TIME_H

#include <nucleus/types.h>

NU_API void  nu_timer_start(nu_timer_t *timer);
NU_API float nu_timer_elapsed(nu_timer_t *timer);

NU_API void      nu_fixed_loop_integrate(nu_loop_t *loops,
                                         nu_size_t  count,
                                         float      dt);
NU_API nu_bool_t nu_fixed_loop_next(nu_loop_t *loops,
                                    nu_size_t  count,
                                    nu_u32_t  *id,
                                    float     *dt);

#ifdef NU_IMPLEMENTATION

#if defined(NU_PLATFORM_WINDOWS)

void
nu_timer_start (nu_timer_t timer)
{
    nu_timer_data_t *p = (nu_timer_data_t *)timer;
    QueryPerformanceCounter(&p->t0);
}
float
nu_timer_elapsed (nu_timer_t timer)
{
    nu_timer_data_t *p = (nu_timer_data_t *)timer;

    LARGE_INTEGER frequency, t1;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    double delta = (t1.QuadPart - p->t0.QuadPart) * 1000.0 / frequency.QuadPart;
    return (float)delta;
}

#elif defined(NU_PLATFORM_UNIX)

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
nu_timer_start (nu_timer_t *timer)
{
    clock_gettime(CLOCK_MONOTONIC, &timer->start);
}
float
nu_timer_elapsed (nu_timer_t *timer)
{
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);

    struct timespec diff;
    timespec_diff(&timer->start, &end, &diff);

    return (double)diff.tv_nsec / 1.0e6;
}
#endif

void
nu_fixed_loop_integrate (nu_loop_t *loops, nu_size_t count, float dt)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_loop_t *loop = &loops[i];
        if (!loop->active)
        {
            continue;
        }
        loop->_acc += dt;
    }
}

nu_bool_t
nu_fixed_loop_next (nu_loop_t *loops, nu_size_t count, nu_u32_t *id, float *dt)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_loop_t *loop = &loops[i];
        if (!loop->active)
        {
            continue;
        }
        if (loop->_acc >= loop->fixed)
        {
            loop->_acc -= loop->fixed;
            *dt = loop->fixed;
            *id = loop->id;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}

#endif

#endif
