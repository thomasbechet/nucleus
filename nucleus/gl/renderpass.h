#ifndef NUGL_RENDERPASS_H
#define NUGL_RENDERPASS_H

#include <nucleus/graphics/api.h>
#include <nucleus/external/glad/gl.h>

#define NUGL__MIN_DEPTH       0.0
#define NUGL__MAX_DEPTH       1000.0
#define NUGL__DEPTH_INCREMENT 0.05

typedef enum
{
    NUGL__DRAW,
    NUGL__DRAW_INSTANCED,
} nugl__mesh_command_type_t;

typedef struct
{
    nugl__mesh_command_type_t type;
    GLuint                    vao;
    nu_size_t                 vcount;
    nu_mat4_t                 transform; // TODO: use indexed UBO
    GLuint                    texture0;
    GLuint                    texture1;
    nu_mat3_t                 uv_transform;
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
    float    depth;
} nugl__gpu_blit_t;

typedef NU_VEC(nugl__gpu_blit_t) nugl__gpu_blit_vec_t;

typedef struct
{
    nugl__mesh_command_vec_t cmds;
    nu_camera_t              camera;
    nu_material_t            material;
} nugl__renderpass_unlit_t;

typedef struct
{
    nugl__mesh_command_vec_t cmds;
    nu_camera_t              camera;
    nu_material_t            material;
} nugl__renderpass_flat_t;

typedef struct
{
    nu_camera_t  camera;
    nu_cubemap_t cubemap;
    nu_mat3_t    rotation;
} nugl__renderpass_skybox_t;

typedef struct
{
    nugl__canvas_command_vec_t cmds;
    nugl__gpu_blit_vec_t       blit_transfer;
    GLuint                     blit_vbo;
    nu_size_t                  blit_vbo_size;
    GLuint                     blit_vao;
    float                      depth;
    nu_material_t              material;
} nugl__renderpass_canvas_t;

typedef struct
{
    nu_camera_t              camera;
    nugl__mesh_command_vec_t cmds;
    nu_color_t               color;
    nu_material_t            material;
} nugl__renderpass_wireframe_t;

typedef struct
{
    nu_renderpass_type_t type;
    nu_texture_t         depth_target;
    nu_texture_t         color_target;
    nu_color_t           clear_color;
    nu_bool_t            has_clear_color;
    GLuint               fbo;
    nu_uvec2_t           fbo_size;
    nu_bool_t            reset_after_submit;
    union
    {
        nugl__renderpass_unlit_t     unlit;
        nugl__renderpass_flat_t      flat;
        nugl__renderpass_skybox_t    skybox;
        nugl__renderpass_canvas_t    canvas;
        nugl__renderpass_wireframe_t wireframe;
    };
} nugl__renderpass_t;

typedef struct
{
    GLuint color;
    GLuint depth;
    GLuint fbo;
} nugl__rendertarget_t;

typedef NU_VEC(nugl__rendertarget_t) nugl__rendertarget_vec_t;
typedef NU_VEC(nugl__renderpass_t) nugl__renderpass_vec_t;

#endif
