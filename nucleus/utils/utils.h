#ifndef NU_UTILS_H
#define NU_UTILS_H

#include <nucleus/utils/geometry.h>
#include <nucleus/utils/controller.h>

typedef struct
{
    nu__camera_controller_pool_t controllers;
    nu__geometry_pool_t          geometries;
} nu__utils_t;

static nu_error_t nu__utils_init(void);
static nu_error_t nu__utils_free(void);

#endif
