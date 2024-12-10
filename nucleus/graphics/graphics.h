#ifndef NU_GRAPHICS_H
#define NU_GRAPHICS_H

#include <nucleus/graphics/font.h>
#include <nucleus/graphics/model.h>
#include <nucleus/graphics/immediate.h>
#include <nucleus/graphics/texture.h>
#include <nucleus/graphics/mesh.h>

typedef struct
{
    nu_size_t triangle_count;
    nu_size_t line_count;
    nu_size_t point_count;
} nugfx__graphics_stats_t;

typedef struct
{
    nugfx_mesh_t mesh;
    nugfx_mesh_t texture;
    nugfx_font_t font;
    nu_m4_t      transform;
} nugfx__state_t;

typedef struct
{
    nu__graphics_immediate_t im;

    nu_object_type_t obj_texture;
    nu_object_type_t obj_mesh;
    nu_object_type_t obj_font;
    nu_object_type_t obj_model;

#ifdef NU_BUILD_GRAPHICS_GL
    nu__gl_t gl;
#endif
} nugfx__module_t;

static nu_error_t nugfx__init(void);
static nu_error_t nugfx__free(void);
static nu_error_t nugfx__render(void);

#endif
