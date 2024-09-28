#ifndef NU_UTILS_STATS_H
#define NU_UTILS_STATS_H

#include <nucleus/graphics/api.h>

#define NU__STATS_FRAME_COUNT 10

typedef struct
{
    nu_timer_t timer;
    float      frames[NU__STATS_FRAME_COUNT];
    nu_size_t  head;
    float      avg;
} nu__stats_t;

static void nu__stats_init(void);
static void nu__stats_update(void);

#endif
