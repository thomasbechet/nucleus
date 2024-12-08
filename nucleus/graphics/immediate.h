#ifndef NU_IMMEDIATE_H
#define NU_IMMEDIATE_H

#include <nucleus/graphics/api.h>

typedef struct
{
    nu_mesh_t points;
    nu_mesh_t lines;
    nu_mesh_t lines_strip;
    nu_mesh_t triangles;
} nu__graphics_immediate_t;

static void nu__graphics_immediate_init(void);
static void nu__graphics_immediate_reset(void);

#endif
