#ifndef NU_TIME_H
#define NU_TIME_H

#include <nucleus/core/config.h>

typedef struct
{
#if defined(NU_PLATFORM_WINDOWS)
    LARGE_INTEGER value;
#elif defined(NU_PLATFORM_UNIX)
    time_t value;
#endif
} nu_time_t;

typedef struct
{
#if defined(NU_PLATFORM_WINDOWS)
    LARGE_INTEGER t0;
#elif defined(NU_PLATFORM_UNIX)
    struct timespec start;
#endif
} nu_timer_t;

typedef struct
{
    nu_bool_t active;
    nu_u32_t  id;
    float     timestep;
    float     _acc;
} nu_fixed_loop_t;

NU_API nu_time_t nu_time(void);
NU_API nu_u32_t  nu_time_hours(const nu_time_t *time);
NU_API nu_u32_t  nu_time_minutes(const nu_time_t *time);
NU_API nu_u32_t  nu_time_seconds(const nu_time_t *time);

NU_API void  nu_timer_reset(nu_timer_t *timer);
NU_API float nu_timer_elapsed(nu_timer_t *timer);

NU_API nu_fixed_loop_t nu_fixed_loop(nu_u32_t id, float timestep);
NU_API nu_bool_t       nu_next_fixed_loop(nu_fixed_loop_t *loops,
                                          nu_size_t        count,
                                          nu_u32_t        *id);
NU_API void            nu_update_fixed_loops(nu_fixed_loop_t *loops,
                                             nu_size_t        count,
                                             float            dt);

#endif
