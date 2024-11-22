#ifndef NU_IMMEDIATE_H
#define NU_IMMEDIATE_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_mesh_t points;
    nu_size_t points_count;
    nu_mesh_t lines;
    nu_size_t lines_count;
    nu_mesh_t lines_strip;
    nu_size_t lines_strip_count;
    nu_mesh_t triangles;
    nu_size_t triangles_count;
} nu__graphics_immediate_t;

static void nu__graphics_immediate_init(void);
static void nu__graphics_immediate_reset(void);

#endif
