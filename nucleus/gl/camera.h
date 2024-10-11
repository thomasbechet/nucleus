#ifndef NUGL_CAMERA_H
#define NUGL_CAMERA_H

#include <nucleus/graphics/api.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    GLuint    ubo;
    nu_m4_t view;
    nu_m4_t projection;
    nu_m4_t vp;
    nu_m4_t ivp;
} nugl__camera_t;

typedef NU_VEC(nugl__camera_t) nugl__camera_vec_t;

#endif
