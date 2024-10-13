#ifndef NU_CAMERA_H
#define NU_CAMERA_H

#include <nucleus/graphics/backend.h>

typedef struct
{
    nu_m4_t view;
    nu_m4_t projection;
    nu_m4_t vp;
    nu_m4_t ivp;
} nu__camera_t;

typedef NU_POOL(nu__camera_t) nu__camera_pool_t;

#endif
