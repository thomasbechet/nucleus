#ifndef NU_UTILS_H
#define NU_UTILS_H

#include <nucleus/utils/controller.h>
#include <nucleus/utils/stats.h>

typedef struct
{
    nu_object_type_t obj_camera_controller;
    nu__stats_t      stats;
} nu__utils_t;

static nu_error_t nu__utils_init(void);

#endif
