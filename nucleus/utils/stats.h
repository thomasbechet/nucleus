#ifndef NU_UTILS_STATS_H
#define NU_UTILS_STATS_H

#include <nucleus/graphics/graphics.h>

#define NU__STATS_FRAME_COUNT 10

typedef struct
{
    nu_timer_t timer;
    nu_f32_t   frames[NU__STATS_FRAME_COUNT];
    nu_size_t  head;
    nu_f32_t   avg;
    nu_size_t  frame;

    nu__graphics_stats_t graphics_current;
    nu__graphics_stats_t graphics_frame;
} nu__stats_t;

static void nu__stats_init(void);
static void nu__stats_update(void);

#endif
