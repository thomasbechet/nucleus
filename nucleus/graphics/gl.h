#ifndef NUGL_H
#define NUGL_H

#include <nucleus/core.h>
#include <nucleus/graphics/shared_types.h>
#include <nucleus/external/glad/gl.h>

#define NUGL__VERTEX_SIZE            (3 + 2)
#define NUGL__MAX_COMMAND_COUNT      128
#define NUGL__MAX_RENDERPASS_COUNT   32
#define NUGL__MAX_RENDERTARGET_COUNT 32

typedef struct
{
    GLuint    ubo;
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
    GLuint    texture0;
    GLuint    texture1;
    nu_mat3_t uv_transform;
} nugl__material_t;

typedef struct
{
    GLuint     texture;
    nu_uvec2_t size;
} nugl__texture_t;

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

typedef struct
{
    nugl__command_t *commands;
    nu_size_t        count;
} nugl__command_buffer_t;

typedef struct
{
    nu_renderpass_type_t   type;
    nugl__command_buffer_t cmds;
    nu_mat4_t              vp;
    nu_mat4_t              ivp;
    nu_color_t             clear_color;
    GLuint                 depth_target;
    GLuint                 color_target;
    GLuint                 fbo;
    nu_uvec2_t             fbo_size;
    nu_bool_t              reset;
} nugl__renderpass_data_t;

typedef struct
{
    nu_u32_t id;
} nugl__renderpass_t;

typedef struct
{
    GLuint color;
    GLuint depth;
    GLuint fbo;
} nugl__rendertarget_t;

typedef struct
{
    nu_allocator_t allocator;
    nu_uvec2_t     surface_size;
    GLuint         surface_color;

    GLuint blit_program;
    GLuint flat_program;
    GLuint nearest_sampler;

    nugl__rendertarget_t targets[NUGL__MAX_RENDERTARGET_COUNT];
    nu_size_t            target_count;

    nugl__renderpass_data_t passes[NUGL__MAX_RENDERPASS_COUNT];
    nu_u32_t                pass_count;
    nu_u32_t                pass_order[NUGL__MAX_RENDERPASS_COUNT];
    nu_u32_t                pass_order_count;
} nugl__context_t;

#endif
