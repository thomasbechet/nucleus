#ifndef NU_GEOMETRY_H
#define NU_GEOMETRY_H

#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_vec3_vec_t positions;
    nu_vec2_vec_t uvs;
    nu_vec3_vec_t normals;
} nu__geometry_t;

typedef NU_POOL(nu__geometry_t) nu__geometry_pool_t;

#endif
