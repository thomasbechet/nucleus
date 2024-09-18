#ifndef NUGL_CAMERA_H
#define NUGL_CAMERA_H

#include <nucleus/core/core.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    GLuint    ubo;
    nu_mat4_t view;
    nu_mat4_t projection;
    nu_mat4_t vp;
    nu_mat4_t ivp;
} nugl__camera_t;

typedef NU_VEC(nugl__camera_t) nugl__camera_vec_t;

#endif
