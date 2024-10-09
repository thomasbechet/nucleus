#ifndef NU_GRAPHICS_H
#define NU_GRAPHICS_H

#include <nucleus/graphics/font.h>
#include <nucleus/graphics/image.h>
#include <nucleus/graphics/model.h>
#include <nucleus/graphics/immediate.h>

typedef struct
{
    nu_size_t triangle_count;
    nu_size_t line_count;
    nu_size_t point_count;
    nu_size_t renderpass_count;
} nu__graphics_stats_t;

typedef struct
{
    nu_texture_t             surface_color;
    nu__font_pool_t          fonts;
    nu__image_pool_t         images;
    nu__model_pool_t         models;
    nu__graphics_immediate_t im;
} nu__graphics_t;

static nu_error_t nu__graphics_init(void);
static nu_error_t nu__graphics_free(void);
static nu_error_t nu__graphics_render(void);

static nu_size_t
nu__primitive_vertex_count (nu_primitive_t primitive, nu_size_t capacity)
{
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            return capacity;
        case NU_PRIMITIVE_LINES:
            return capacity * 2;
        case NU_PRIMITIVE_LINES_STRIP:
            return capacity + 1;
        case NU_PRIMITIVE_TRIANGLES:
            return capacity * 3;
    }
    return 0;
}
static nu_size_t
nu__primitive_vertex_first (nu_primitive_t primitive, nu_size_t first)
{
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            return first;
        case NU_PRIMITIVE_LINES:
            return first * 2;
        case NU_PRIMITIVE_LINES_STRIP:
            return first;
        case NU_PRIMITIVE_TRIANGLES:
            return first * 3;
    }
    return 0;
}

#endif
