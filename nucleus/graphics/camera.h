#ifndef NU_CAMERA_H
#define NU_CAMERA_H

#include <nucleus/graphics/backend.h>

typedef struct
{
#ifdef NU_BUILD_GL
    nugl__camera_t gl;
#endif
    nu_m4_t view;
    nu_m4_t projection;
    nu_m4_t vp;
    nu_m4_t ivp;
} nu__camera_t;

typedef NU_VEC(nu__camera_t) nu__camera_vec_t;

#endif
