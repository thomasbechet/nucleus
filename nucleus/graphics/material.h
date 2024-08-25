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

#define NU_MATERIAL_INFO_DEFAULT_MESH                                   \
    (nu_material_info_t)                                                \
    {                                                                   \
        .type = NU_MATERIAL_MESH, .mesh.color0 = NU_NULL,               \
        .mesh.color1 = NU_NULL, .mesh.uv_transform = nu_mat3_identity() \
    }
#define NU_MATERIAL_INFO_DEFAULT_CANVAS                       \
    (nu_material_info_t)                                      \
    {                                                         \
        .type = NU_MATERIAL_CANVAS, .canvas.color0 = NU_NULL, \
        .canvas.wrap_mode = NU_TEXTURE_WRAP_CLAMP             \
    }

NU_DEFINE_HANDLE(nu_material_handle_t);

#endif
