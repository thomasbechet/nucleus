#ifndef NU_MATERIAL_H
#define NU_MATERIAL_H

#include <nucleus/graphics/texture.h>

typedef enum
{
    NU_MATERIAL_MESH,
    NU_MATERIAL_CANVAS,
} nu_material_type_t;

typedef struct
{
    const nu_texture_handle_t *color0;
    const nu_texture_handle_t *color1;
    nu_mat3_t                  uv_transform;
} nu_material_mesh_t;

typedef struct
{
    const nu_texture_handle_t *color0;
    nu_texture_wrap_mode_t     wrap_mode;
} nu_material_canvas_t;

typedef struct
{
    nu_material_type_t type;
    union
    {
        nu_material_mesh_t   mesh;
        nu_material_canvas_t canvas;
    };
} nu_material_info_t;

NU_DEFINE_HANDLE(nu_material_handle_t);

#endif
