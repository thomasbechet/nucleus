#ifndef NUGL_MATERIAL_H
#define NUGL_MATERIAL_H

#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_color_t   color;
    nu_texture_t texture0;
    nu_texture_t texture1;
    nu_mat3_t    uv_transform;
} nugl__material_mesh_t;

typedef struct
{
    nu_texture_t           texture0;
    nu_texture_wrap_mode_t wrap_mode;
} nugl__material_canvas_t;

typedef struct
{
    nu_material_type_t type;
    union
    {
        nugl__material_mesh_t   mesh;
        nugl__material_canvas_t canvas;
    };
} nugl__material_t;

typedef NU_VEC(nugl__material_t) nugl__material_vec_t;

#endif
