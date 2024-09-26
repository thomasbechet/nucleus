#ifndef NU_GRAPHICS_H
#define NU_GRAPHICS_H

#include <nucleus/graphics/renderer.h>
#include <nucleus/graphics/font.h>
#include <nucleus/graphics/image.h>
#include <nucleus/graphics/model.h>

typedef struct
{
    nu_renderer_api_t api;
} nu__graphics_config_t;

typedef struct
{
    nu_mesh_t points;
    nu_size_t points_count;
    nu_mesh_t lines;
    nu_size_t lines_count;
    nu_mesh_t triangles;
    nu_size_t triangles_count;
} nu__graphics_immediate_t;

typedef struct
{
    nu__renderer_t           renderer;
    nu__font_pool_t          fonts;
    nu__image_pool_t         images;
    nu__model_pool_t         models;
    nu__graphics_immediate_t im;
} nu__graphics_t;

static nu_error_t nu__graphics_init(void);
static nu_error_t nu__graphics_free(void);
static nu_error_t nu__graphics_render(void);

#endif
