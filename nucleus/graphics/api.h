#ifndef NU_GRAPHICS_API_H
#define NU_GRAPHICS_API_H

#include <nucleus/platform/api.h>

//////////////////////////////////////////////////////////////////////////
//////                         Image                                //////
//////////////////////////////////////////////////////////////////////////

NU_DEFINE_HANDLE(nu_image_t);

NU_API nu_image_t  nu_image_create(nu_uvec2_t size);
NU_API void        nu_image_delete(nu_image_t image);
NU_API nu_color_t *nu_image_colors(nu_image_t image);
NU_API nu_uvec2_t  nu_image_size(nu_image_t image);

//////////////////////////////////////////////////////////////////////////
//////                           Camera                             //////
//////////////////////////////////////////////////////////////////////////

NU_DEFINE_HANDLE(nu_camera_t);

NU_API nu_camera_t nu_camera_create(void);
NU_API void        nu_camera_delete(nu_camera_t camera);
NU_API void        nu_camera_view(nu_camera_t camera,
                                  nu_vec3_t   up,
                                  nu_vec3_t   center,
                                  nu_vec3_t   eye);
NU_API void        nu_camera_perspective(nu_camera_t camera,
                                         float       fov,
                                         float       near,
                                         float       far);
NU_API void        nu_camera_ortho(nu_camera_t camera);

//////////////////////////////////////////////////////////////////////////
//////                          Texture                             //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_TEXTURE_FORMAT_COLOR,
    NU_TEXTURE_FORMAT_DEPTH
} nu_texture_format_t;

typedef enum
{
    NU_TEXTURE_USAGE_TARGET,
    NU_TEXTURE_USAGE_SAMPLE
} nu_texture_usage_t;

typedef enum
{
    NU_TEXTURE_WRAP_CLAMP,
    NU_TEXTURE_WRAP_REPEAT,
    NU_TEXTURE_WRAP_MIRROR
} nu_texture_wrap_mode_t;

NU_DEFINE_HANDLE(nu_texture_t);

NU_API nu_texture_t nu_texture_create(nu_uvec2_t          size,
                                      nu_texture_format_t format,
                                      nu_texture_usage_t  usage,
                                      const nu_color_t   *colors);
NU_API nu_texture_t nu_texture_create_color(nu_color_t color);
NU_API nu_texture_t nu_texture_create_image(nu_image_t image);
NU_API nu_error_t   nu_texture_delete(nu_texture_t texture);

//////////////////////////////////////////////////////////////////////////
//////                          Cubemap                             //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_u32_t           size;
    nu_texture_usage_t usage;
    const nu_color_t  *colors_posx;
    const nu_color_t  *colors_negx;
    const nu_color_t  *colors_posy;
    const nu_color_t  *colors_negy;
    const nu_color_t  *colors_posz;
    const nu_color_t  *colors_negz;
} nu_cubemap_info_t;

#define NU_CUBEMAP_FACE_COUNT 6

NU_DEFINE_HANDLE(nu_cubemap_t);

NU_API nu_cubemap_t nu_cubemap_create(const nu_cubemap_info_t *info);
NU_API nu_error_t   nu_cubemap_delete(nu_cubemap_t cubemap);

//////////////////////////////////////////////////////////////////////////
//////                         Model                                //////
//////////////////////////////////////////////////////////////////////////

NU_DEFINE_HANDLE(nu_model_t);

NU_API nu_model_t nu_model_create(void);
NU_API void       nu_model_delete(nu_model_t model);

//////////////////////////////////////////////////////////////////////////
//////                          Font                                //////
//////////////////////////////////////////////////////////////////////////

NU_DEFINE_HANDLE(nu_font_t);

NU_API nu_font_t nu_font_create_default(void);
NU_API void      nu_font_delete(nu_font_t font);

//////////////////////////////////////////////////////////////////////////
//////                          Mesh                                //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    const nu_vec3_t *positions;
    const nu_vec2_t *uvs;
    const nu_vec3_t *normals;
    nu_size_t        count;
} nu_mesh_info_t;

NU_DEFINE_HANDLE(nu_mesh_t);

NU_API nu_mesh_t  nu_mesh_create(const nu_mesh_info_t *info);
NU_API nu_error_t nu_mesh_delete(nu_mesh_t mesh);

//////////////////////////////////////////////////////////////////////////
//////                        Material                              //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_MATERIAL_MESH,
    NU_MATERIAL_CANVAS,
} nu_material_type_t;

typedef struct
{
    nu_material_type_t type;
    union
    {
        struct
        {
            const nu_texture_t *color0;
            const nu_texture_t *color1;
            nu_mat3_t           uv_transform;
        } mesh;
        struct
        {
            const nu_texture_t    *color0;
            nu_texture_wrap_mode_t wrap_mode;
        } canvas;
    };
} nu_material_info_t;

NU_DEFINE_HANDLE(nu_material_t);

NU_API nu_material_info_t nu_material_info_default(nu_material_type_t type);
NU_API nu_material_t      nu_material_create(const nu_material_info_t *info);
NU_API nu_error_t         nu_material_delete(nu_material_t material);
NU_API nu_error_t         nu_material_update(nu_material_t             material,
                                             const nu_material_info_t *info);

//////////////////////////////////////////////////////////////////////////
//////                        Renderpass                            //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_RENDERPASS_UNLIT,
    NU_RENDERPASS_FLAT,
    NU_RENDERPASS_TRANSPARENT,
    NU_RENDERPASS_WIREFRAME,
    NU_RENDERPASS_SKYBOX,
    NU_RENDERPASS_CANVAS,
} nu_renderpass_type_t;

typedef struct
{
    nu_renderpass_type_t type;
    nu_bool_t            reset_after_submit;
    union
    {
        nu_u32_t todo;
    };
} nu_renderpass_info_t;

typedef union
{
    struct
    {
        nu_camera_t         camera;
        const nu_texture_t *color_target;
        const nu_texture_t *depth_target;
        nu_color_t         *clear_color;
        nu_cubemap_t       *clear_skybox;
    } unlit;
    struct
    {
        nu_camera_t         camera;
        const nu_texture_t *color_target;
        const nu_texture_t *depth_target;
        nu_color_t         *clear_color;
    } flat;
    struct
    {
        nu_camera_t         camera;
        const nu_texture_t *color_target;
        const nu_texture_t *depth_target;
        nu_cubemap_t        cubemap;
        nu_quat_t           rotation;
    } skybox;
    struct
    {
        const nu_texture_t *color_target;
    } canvas;
} nu_renderpass_submit_t;

NU_DEFINE_HANDLE(nu_renderpass_t);

NU_API nu_renderpass_t nu_renderpass_create(const nu_renderpass_info_t *info);
NU_API nu_error_t      nu_renderpass_delete(nu_renderpass_t pass);

NU_API void nu_renderpass_submit(nu_renderpass_t               pass,
                                 const nu_renderpass_submit_t *info);
NU_API void nu_renderpass_reset(nu_renderpass_t pass);
NU_API void nu_draw_mesh(nu_renderpass_t pass,
                         nu_material_t   material,
                         nu_mesh_t       mesh,
                         nu_mat4_t       transform);
NU_API void nu_draw_blit(nu_renderpass_t pass,
                         nu_material_t   material,
                         nu_rect_t       extent,
                         nu_rect_t       tex_extent);
NU_API void nu_draw_model(nu_renderpass_t pass,
                          nu_model_t      model,
                          nu_mat4_t       transform);
NU_API void nu_draw_text(nu_renderpass_t  pass,
                         nu_font_t        font,
                         const nu_char_t *text,
                         nu_size_t        n,
                         nu_ivec2_t       pos);

//////////////////////////////////////////////////////////////////////////
//////                        Renderer                              //////
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    NU_RENDERER_NULL,
    NU_RENDERER_GL,
    NU_RENDERER_DX11,
    NU_RENDERER_SOFTRAST,
} nu_renderer_api_t;

NU_API void         nu_config_renderer_api(nu_renderer_api_t api);
NU_API nu_texture_t nu_surface_color_target(void);

#endif
