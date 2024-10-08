#ifndef NUGL_H
#define NUGL_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/gl/shader_data.h>
#include <nucleus/gl/texture.h>
#include <nucleus/gl/material.h>
#include <nucleus/gl/light.h>
#include <nucleus/gl/mesh.h>
#include <nucleus/gl/camera.h>
#include <nucleus/gl/renderpass.h>

typedef struct
{
    nu_u32_t surface_color_index;

    GLuint screen_blit_program;
    GLuint unlit_program;
    GLuint lit_program;
    GLuint skybox_program;
    GLuint wireframe_program;
    GLuint canvas_blit_program;
    GLuint shadow_program;
    GLuint nearest_sampler;

    nugl__camera_vec_t       cameras;
    nugl__mesh_vec_t         meshes;
    nugl__texture_vec_t      textures;
    nugl__material_vec_t     materials;
    nugl__light_vec_t        lights;
    nugl__rendertarget_vec_t targets;
    nugl__renderpass_vec_t   passes;
    nu_u32_vec_t             passes_order;
} nu__gl_t;

#endif
