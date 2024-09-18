#ifndef NUGL_H
#define NUGL_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/gl/shader_data.h>
#include <nucleus/gl/texture.h>
#include <nucleus/gl/material.h>
#include <nucleus/gl/mesh.h>
#include <nucleus/gl/camera.h>
#include <nucleus/gl/renderpass.h>

typedef struct
{
    nu_u32_t surface_color_index;

    GLuint blit_program;
    GLuint flat_program;
    GLuint skybox_program;
    GLuint canvas_blit_program;
    GLuint nearest_sampler;

    nugl__camera_vec_t       cameras;
    nugl__mesh_vec_t         meshes;
    nugl__texture_vec_t      textures;
    nugl__cubemap_vec_t      cubemaps;
    nugl__material_vec_t     materials;
    nugl__rendertarget_vec_t targets;
    nugl__renderpass_vec_t   passes;
    nu_u32_vec_t             passes_order;
} nu__gl_t;

static void nugl__setup_api(nu__renderer_api_t *api);

#endif
