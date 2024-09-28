#ifndef NU_UTILS_H
#define NU_UTILS_H

#include <nucleus/utils/geometry.h>
#include <nucleus/utils/controller.h>
#include <nucleus/utils/stats.h>

typedef struct
{
    nu__camera_controller_pool_t controllers;
    nu__geometry_pool_t          geometries;
    nu__stats_t                  stats;
} nu__utils_t;

static nu_error_t nu__utils_init(void);
static void       nu__utils_free(void);

#endif
