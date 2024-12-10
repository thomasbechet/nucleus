#ifndef NU_IMMEDIATE_H
#define NU_IMMEDIATE_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nugfx_mesh_t points;
    nugfx_mesh_t lines;
    nugfx_mesh_t lines_strip;
    nugfx_mesh_t triangles;
} nu__graphics_immediate_t;

static void nu__graphics_immediate_init(void);
static void nu__graphics_immediate_reset(void);

#endif
