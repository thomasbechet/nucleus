#ifndef NUGL_H
#define NUGL_H

#include <nucleus/core.h>
#include <nucleus/graphics/shared_types.h>
#include <nucleus/external/glad/gl.h>

#define NUGL__VERTEX_SIZE (3 + 2 + 3)

typedef struct
{
    GLuint    ubo;
    nu_mat4_t view;
    nu_mat4_t projection;
    nu_mat4_t vp;
    nu_mat4_t ivp;
} nugl__camera_t;

typedef struct
{
    GLuint    vao;
    GLuint    vbo;
    nu_size_t vertex_count;
} nugl__mesh_t;

typedef struct
{
    nu_uvec2_t size;
    GLuint     texture;
} nugl__texture_t;

typedef struct
{
    GLuint texture;
} nugl__cubemap_t;

typedef struct
{
    GLuint    texture0;
    GLuint    texture1;
    nu_mat3_t uv_transform;
} nugl__material_t;

typedef enum
{
    NUGL__DRAW,
    NUGL__DRAW_INSTANCED,
} nugl__command_type_t;

typedef struct
{
    nugl__command_type_t type;
    GLuint               vao;
    GLuint               vbo;
    nu_size_t            vcount;
    nu_mat4_t            transform; // TODO: remove me
    GLuint               texture0;
    GLuint               texture1;
    nu_mat3_t            uv_transform;
} nugl__command_t;

typedef nu_vec(nugl__command_t) nugl__command_vec_t;

typedef struct
{
    nu_color_t clear_color;
    nu_bool_t  has_clear_color;
} nugl__renderpass_flat_t;

typedef struct
{
    GLuint    cubemap;
    nu_mat3_t rotation;
} nugl__renderpass_skybox_t;

typedef struct
{
    nu_renderpass_type_t type;
    nugl__command_vec_t  cmds;
    nu_u32_t             camera;
    GLuint               depth_target;
    GLuint               color_target;
    GLuint               fbo;
    nu_uvec2_t           fbo_size;
    nu_bool_t            reset_after_submit;
    union
    {
        nugl__renderpass_flat_t   flat;
        nugl__renderpass_skybox_t skybox;
    };
} nugl__renderpass_t;

typedef struct
{
    GLuint color;
    GLuint depth;
    GLuint fbo;
} nugl__rendertarget_t;

typedef union
{
    nu_u32_t index;
} nugl__handle_t;

typedef nu_vec(nugl__camera_t) nugl__camera_vec_t;
typedef nu_vec(nugl__mesh_t) nugl__mesh_vec_t;
typedef nu_vec(nugl__texture_t) nugl__texture_vec_t;
typedef nu_vec(nugl__cubemap_t) nugl__cubemap_vec_t;
typedef nu_vec(nugl__material_t) nugl__material_vec_t;
typedef nu_vec(nugl__rendertarget_t) nugl__rendertarget_vec_t;
typedef nu_vec(nugl__renderpass_t) nugl__renderpass_vec_t;

typedef struct
{
    nu_allocator_t allocator;
    nu_u32_t       surface_color_index;

    GLuint blit_program;
    GLuint flat_program;
    GLuint skybox_program;
    GLuint nearest_sampler;

    nugl__camera_vec_t       cameras;
    nugl__mesh_vec_t         meshes;
    nugl__texture_vec_t      textures;
    nugl__cubemap_vec_t      cubemaps;
    nugl__material_vec_t     materials;
    nugl__rendertarget_vec_t targets;
    nugl__renderpass_vec_t   passes;
    nu_u32_vec_t             passes_order;
} nugl__context_t;

#endif
