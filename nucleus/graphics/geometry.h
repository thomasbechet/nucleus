#ifndef NU_GEOMETRY_H
#define NU_GEOMETRY_H

#include <nucleus/utils/api.h>

typedef struct
{
    nu_geometry_type_t type;
    union
    {
        struct
        {
            NU_VEC(nu_v3_t) positions;
            NU_VEC(nu_v2_t) uvs;
            NU_VEC(nu_u16_t) positions_indices;
            NU_VEC(nu_u16_t) uvs_indices;
            nu_primitive_t primitive;
        } mesh;
    };
} nu__geometry_t;

#endif
