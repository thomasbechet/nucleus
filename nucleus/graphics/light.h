#ifndef NU_LIGHT_H
#define NU_LIGHT_H

#include <nucleus/graphics/api.h>
#include <nucleus/graphics/backend.h>

typedef struct
{
    nu_light_type_t type;
    nu_v3_t         position;
    nu_q4_t         rotation;
} nu__light_t;

typedef NU_VEC(nu__light_t) nu__light_vec_t;

typedef struct
{
    nu_texture_t shadowmap;
    nu_camera_t  shadowmap_camera;
    nu_texture_t skybox;
    nu_m3_t      skybox_rotation;
} nu__lightenv_t;

typedef NU_VEC(nu__lightenv_t) nu__lightenv_vec_t;

#endif
