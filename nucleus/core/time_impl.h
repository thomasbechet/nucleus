#ifndef NU_TIME_IMPL_H
#define NU_TIME_IMPL_H

#include <nucleus/internal.h>

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

nu_fixed_loop_t
nu_fixed_loop (nu_u32_t id, float timestep)
{
    nu_fixed_loop_t loop;
    loop.id       = id;
    loop.timestep = timestep;
    loop.active   = NU_TRUE;
    loop._acc     = 0.0f;
    return loop;
}
void
nu_fixed_loop_update (nu_fixed_loop_t *loops, nu_size_t count, float dt)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_fixed_loop_t *loop = &loops[i];
        if (!loop->active)
        {
            continue;
        }
        loop->_acc += dt;
    }
}
nu_bool_t
nu_fixed_loop_next (nu_fixed_loop_t *loops, nu_size_t count, nu_u32_t *id)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_fixed_loop_t *loop = &loops[i];
        if (!loop->active)
        {
            continue;
        }
        if (loop->_acc >= loop->timestep)
        {
            loop->_acc -= loop->timestep;
            *id = loop->id;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}

#endif
