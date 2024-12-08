#ifndef NU_GRAPHICS_H
#define NU_GRAPHICS_H

#include <nucleus/graphics/font.h>
#include <nucleus/graphics/image.h>
#include <nucleus/graphics/model.h>
#include <nucleus/graphics/immediate.h>
#include <nucleus/graphics/camera.h>
#include <nucleus/graphics/texture.h>
#include <nucleus/graphics/material.h>
#include <nucleus/graphics/mesh.h>
#include <nucleus/graphics/light.h>
#include <nucleus/graphics/renderpass.h>

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
    nu__graphics_immediate_t im;

    nu_object_type_t obj_font;
    nu_object_type_t obj_image;
    nu_object_type_t obj_model;
    nu_object_type_t obj_camera;
    nu_object_type_t obj_texture;
    nu_object_type_t obj_material;
    nu_object_type_t obj_mesh;
    nu_object_type_t obj_light;
    nu_object_type_t obj_lightenv;
    nu_object_type_t obj_renderpass;

#ifdef NU_BUILD_GRAPHICS_GL
    nu__gl_t gl;
#endif
} nu__graphics_t;

static nu_error_t nu__graphics_init(void);
static nu_error_t nu__graphics_free(void);
static nu_error_t nu__graphics_render(void);

#endif
