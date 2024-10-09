#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_size_t triangle_count;
    nu_size_t line_count;
    nu_size_t point_count;
    nu_size_t renderpass_count;
} nu__renderer_stats_t;

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
