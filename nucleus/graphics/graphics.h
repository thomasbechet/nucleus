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
    nu_texture_t surface_color;
    NU_POOL(nu__font_t) fonts;
    NU_POOL(nu__model_t) models;
    nu__graphics_immediate_t im;

    NU_POOL(nu__light_t) lights;
    NU_POOL(nu__lightenv_t) lightenvs;
    NU_POOL(nu__renderpass_t) passes;

    nu_object_t obj_font;
    nu_object_t obj_image;
    nu_object_t obj_model;
    nu_object_t obj_camera;
    nu_object_t obj_texture;
    nu_object_t obj_material;
    nu_object_t obj_mesh;
    nu_object_t obj_light;
    nu_object_t obj_lightenv;
    nu_object_t obj_renderpass;

#ifdef NU_BUILD_GRAPHICS_GL
    nu__gl_t gl;
#endif
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
