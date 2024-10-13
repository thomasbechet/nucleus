#ifndef NUGL_LIGHT_H
#define NUGL_LIGHT_H

#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_light_type_t type;
    nu_v3_t         position;
    nu_q4_t         rotation;
} nugl__light_t;

typedef NU_VEC(nugl__light_t) nugl__light_vec_t;

typedef struct
{
    nu_texture_t shadowmap;
    nu_camera_t  shadowmap_camera;
    nu_texture_t skybox;
    nu_m3_t      skybox_rotation;
} nugl__lightenv_t;

typedef NU_VEC(nugl__lightenv_t) nugl__lightenv_vec_t;

#endif
