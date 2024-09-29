#ifndef NUGL_MATERIAL_H
#define NUGL_MATERIAL_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/external/glad/gl.h>

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
        nugl__material_mesh_t   surface;
        nugl__material_canvas_t canvas;
    };
} nugl__material_t;

typedef NU_VEC(nugl__material_t) nugl__material_vec_t;

static nu_mat3_t  nugl__material_surface_uv_transform(nu_material_t mat);
static GLuint     nugl__material_surface_texture0(nu_material_t mat);
static nu_color_t nugl__material_surface_color(nu_material_t mat,
                                               nu_color_t    vdefault);

#endif
