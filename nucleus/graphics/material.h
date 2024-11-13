#ifndef NU_MATERIAL_H
#define NU_MATERIAL_H

#include <nucleus/graphics/backend.h>

typedef struct
{
    nu_color_t   color;
    nu_texture_t texture0;
    nu_texture_t texture1;
    nu_m3_t      uv_transform;
} nu__material_mesh_t;

typedef struct
{
    nu_texture_t           texture0;
    nu_texture_wrap_mode_t wrap_mode;
} nu__material_canvas_t;

typedef struct
{
    nu_material_type_t type;
    union
    {
        nu__material_mesh_t   surface;
        nu__material_canvas_t canvas;
    };
} nu__material_t;

static nu_m3_t    nu__material_surface_uv_transform(nu_material_t mat);
static GLuint     nu__material_surface_texture0(nu_material_t mat);
static nu_color_t nu__material_surface_color(nu_material_t mat,
                                             nu_color_t    vdefault);

#endif
