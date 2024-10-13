#ifndef NUGL_H
#define NUGL_H

#include <nucleus/graphics/api.h>
#include <nucleus/external/glad/gl.h>

typedef struct
{
    GLuint texture;
} nugl__texture_t;

typedef struct
{
    GLuint vao;
    GLuint positions;
    GLuint uvs;
    GLuint normals;
} nugl__mesh_t;

#define NUGL__MIN_DEPTH       0.0
#define NUGL__MAX_DEPTH       1000.0
#define NUGL__DEPTH_INCREMENT 0.05

typedef struct
{
    GLuint        vao;
    nu_size_t     vfirst;
    nu_size_t     vcount;
    nu_m4_t       transform; // TODO: use indexed UBO
    GLuint        primitive;
    nu_material_t material;
} nugl__mesh_command_t;

typedef NU_VEC(nugl__mesh_command_t) nugl__mesh_command_vec_t;

typedef enum
{
    NUGL__CANVAS_BLIT,
} nugl__canvas_command_type_t;

typedef struct
{
    GLuint   texture;
    nu_u32_t instance_start;
    nu_u32_t instance_count;
} nugl__blit_batch_t;

typedef struct
{
    nugl__canvas_command_type_t type;
    union
    {
        nugl__blit_batch_t blit;
    };
} nugl__canvas_command_t;

typedef NU_VEC(nugl__canvas_command_t) nugl__canvas_command_vec_t;

typedef struct
{
    nu_u32_t pos;
    nu_u32_t tex;
    nu_u32_t size;
    nu_f32_t depth;
} nugl__gpu_blit_t;

typedef NU_VEC(nugl__gpu_blit_t) nugl__gpu_blit_vec_t;

typedef struct
{
    nugl__mesh_command_vec_t cmds;
    GLuint                   program;
} nugl__renderpass_forward_t;

typedef struct
{
    nugl__canvas_command_vec_t cmds;
    nugl__gpu_blit_vec_t       blit_transfer;
    GLuint                     blit_vbo;
    nu_size_t                  blit_vbo_size;
    GLuint                     blit_vao;
    nu_f32_t                   depth;
} nugl__renderpass_canvas_t;

typedef struct
{
    nugl__mesh_command_vec_t cmds;
    GLuint                   fbo;
} nugl__renderpass_shadow_t;

typedef struct
{
    GLuint   fbo;
    nu_v2u_t fbo_size;
    union
    {
        nugl__renderpass_forward_t forward;
        nugl__renderpass_canvas_t  canvas;
        nugl__renderpass_shadow_t  shadow;
    };
} nugl__renderpass_t;

typedef struct
{
    GLuint color;
    GLuint depth;
    GLuint fbo;
} nugl__rendertarget_t;

typedef NU_VEC(nugl__rendertarget_t) nugl__rendertarget_vec_t;

static void nugl__submesh_draw_instanced(nugl__mesh_command_vec_t *cmds,
                                         const nu__mesh_t         *pmesh,
                                         nu_size_t                 first,
                                         nu_size_t                 count,
                                         nu_material_t             mat,
                                         const nu_m4_t            *transforms,
                                         nu_size_t instance_count);

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

    nugl__rendertarget_vec_t targets;
    nu_u32_vec_t             passes_order;
} nu__gl_t;

#endif
