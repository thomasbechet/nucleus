#ifndef NU_GEOMETRY_H
#define NU_GEOMETRY_H

#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_size_t    count; // 1 for points, 2 for lines, etc...
    nu_u32_vec_t positions;
    nu_u32_vec_t uvs;
} nu__primitive_type_t;

typedef NU_VEC(nu__primitive_type_t) nu__primitive_type_vec_t;

typedef struct
{
    nu_v3_vec_t              positions;
    nu_v2_vec_t              uvs;
    nu__primitive_type_vec_t primitives;
} nu__geometry_t;

typedef NU_POOL(nu__geometry_t) nu__geometry_pool_t;

#endif
