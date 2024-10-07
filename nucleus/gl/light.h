#ifndef NUGL_LIGHT_H
#define NUGL_LIGHT_H

#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_light_type_t type;
} nugl__light_t;

typedef NU_VEC(nugl__light_t) nugl__light_vec_t;

#endif
