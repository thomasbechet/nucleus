#ifndef NU_GRAPHICS_API_H
#define NU_GRAPHICS_API_H

#include <nucleus/platform/api.h>

NU_DEFINE_HANDLE(nu_image_t);
NU_DEFINE_HANDLE(nu_camera_t);
NU_DEFINE_HANDLE(nu_texture_t);
NU_DEFINE_HANDLE(nu_cubemap_t);
NU_DEFINE_HANDLE(nu_model_t);
NU_DEFINE_HANDLE(nu_font_t);
NU_DEFINE_HANDLE(nu_mesh_t);
NU_DEFINE_HANDLE(nu_material_t);
NU_DEFINE_HANDLE(nu_renderpass_t);

#define NU_CUBEMAP_FACE_COUNT 6

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

typedef enum
{
    NU_MATERIAL_TYPE_SURFACE,
    NU_MATERIAL_TYPE_CANVAS,
} nu_material_type_t;

typedef enum
{
    NU_MATERIAL_COLOR,
    NU_MATERIAL_TEXTURE0,
    NU_MATERIAL_TEXTURE1,
    NU_MATERIAL_UV_TRANSFORM,
    NU_MATERIAL_WRAP_MODE,
} nu_material_property_t;

typedef enum
{
    NU_RENDERPASS_COLOR_TARGET,
    NU_RENDERPASS_DEPTH_TARGET,
    NU_RENDERPASS_CLEAR_COLOR,
    NU_RENDERPASS_CAMERA,
    NU_RENDERPASS_SKYBOX,
    NU_RENDERPASS_SKYBOX_ROTATION
} nu_renderpass_property_t;

typedef enum
{
    NU_PRIMITIVE_POINTS,
    NU_PRIMITIVE_TRIANGLES,
    NU_PRIMITIVE_LINES,
} nu_primitive_t;

typedef enum
{
    NU_MESH_POSITIONS,
    NU_MESH_UVS,
    NU_MESH_NORMALS,
} nu_mesh_buffer_t;

typedef enum
{
    NU_RENDERPASS_TYPE_UNLIT,
    NU_RENDERPASS_TYPE_FLAT,
    NU_RENDERPASS_TYPE_SKYBOX,
    NU_RENDERPASS_TYPE_CANVAS,
    NU_RENDERPASS_TYPE_WIREFRAME
} nu_renderpass_type_t;

typedef enum
{
    NU_RENDERER_NULL,
    NU_RENDERER_GL,
} nu_renderer_api_t;

typedef enum
{
    NU_CUBEMAP_POSX = 0,
    NU_CUBEMAP_NEGX = 1,
    NU_CUBEMAP_POSY = 2,
    NU_CUBEMAP_NEGY = 3,
    NU_CUBEMAP_POSZ = 4,
    NU_CUBEMAP_NEGZ = 5
} nu_cubemap_face_t;

NU_API void         nu_config_renderer_api(nu_renderer_api_t api);
NU_API nu_texture_t nu_surface_color_target(void);

NU_API nu_camera_t nu_camera_create(void);
NU_API void        nu_camera_delete(nu_camera_t camera);
NU_API void        nu_camera_view(nu_camera_t camera, nu_mat4_t view);
NU_API void        nu_camera_proj(nu_camera_t camera, nu_mat4_t proj);

NU_API nu_texture_t nu_texture_create(nu_vec2u_t          size,
                                      nu_texture_format_t format,
                                      nu_texture_usage_t  usage,
                                      const nu_color_t   *colors);
NU_API nu_texture_t nu_texture_create_color(nu_color_t color);
NU_API void         nu_texture_delete(nu_texture_t texture);

NU_API nu_cubemap_t nu_cubemap_create(nu_u32_t           size,
                                      nu_texture_usage_t usage,
                                      const nu_color_t **colors);
NU_API void         nu_cubemap_delete(nu_cubemap_t cubemap);

NU_API nu_mesh_t nu_mesh_create(nu_primitive_t primitive, nu_size_t count);
NU_API void      nu_mesh_delete(nu_mesh_t mesh);
NU_API void      nu_mesh_buffer_vec2(nu_mesh_t        mesh,
                                     nu_mesh_buffer_t buffer,
                                     nu_size_t        first,
                                     nu_size_t        count,
                                     const nu_vec2_t *data);
NU_API void      nu_mesh_buffer_vec3(nu_mesh_t        mesh,
                                     nu_mesh_buffer_t buffer,
                                     nu_size_t        first,
                                     nu_size_t        count,
                                     const nu_vec3_t *data);

NU_API nu_material_t nu_material_create(nu_material_type_t type);
NU_API void          nu_material_delete(nu_material_t material);
NU_API void          nu_material_color(nu_material_t          material,
                                       nu_material_property_t prop,
                                       nu_color_t             color);
NU_API void          nu_material_texture(nu_material_t          material,
                                         nu_material_property_t prop,
                                         nu_texture_t           texture);
NU_API void          nu_material_mat3(nu_material_t          material,
                                      nu_material_property_t prop,
                                      nu_mat3_t              mat);
NU_API void          nu_material_wrap_mode(nu_material_t          material,
                                           nu_material_property_t prop,
                                           nu_texture_wrap_mode_t mode);

NU_API nu_renderpass_t nu_renderpass_create(nu_renderpass_type_t type,
                                            nu_bool_t reset_after_submit);
NU_API void            nu_renderpass_delete(nu_renderpass_t pass);
NU_API void            nu_renderpass_reset(nu_renderpass_t pass);
NU_API void            nu_renderpass_submit(nu_renderpass_t pass);

NU_API void nu_renderpass_color(nu_renderpass_t          pass,
                                nu_renderpass_property_t prop,
                                nu_color_t              *color);
NU_API void nu_renderpass_camera(nu_renderpass_t          pass,
                                 nu_renderpass_property_t prop,
                                 nu_camera_t              camera);
NU_API void nu_renderpass_cubemap(nu_renderpass_t          pass,
                                  nu_renderpass_property_t prop,
                                  nu_cubemap_t             cubemap);
NU_API void nu_renderpass_quat(nu_renderpass_t          pass,
                               nu_renderpass_property_t prop,
                               nu_quat_t                rot);
NU_API void nu_renderpass_texture(nu_renderpass_t          pass,
                                  nu_renderpass_property_t prop,
                                  nu_texture_t             texture);

NU_API void nu_bind_material(nu_renderpass_t pass, nu_material_t material);
NU_API void nu_draw_meshes(nu_renderpass_t  pass,
                           nu_mesh_t        mesh,
                           const nu_mat4_t *transforms,
                           nu_size_t        count);
NU_API void nu_draw_blit(nu_renderpass_t pass,
                         nu_box2i_t      extent,
                         nu_box2i_t      tex_extent);

NU_API void nu_draw_mesh(nu_renderpass_t pass,
                         nu_mesh_t       mesh,
                         nu_mat4_t       transform);
NU_API void nu_draw_model(nu_renderpass_t pass,
                          nu_model_t      model,
                          nu_mat4_t       transform);
NU_API void nu_draw_lines(nu_renderpass_t  pass,
                          const nu_vec3_t *points,
                          nu_color_t       color);
NU_API void nu_draw_text(nu_renderpass_t  pass,
                         nu_font_t        font,
                         const nu_char_t *text,
                         nu_size_t        n,
                         nu_vec2i_t       pos);

NU_API nu_image_t   nu_image_create(nu_vec2u_t size);
NU_API void         nu_image_delete(nu_image_t image);
NU_API nu_color_t  *nu_image_colors(nu_image_t image);
NU_API nu_vec2u_t   nu_image_size(nu_image_t image);
NU_API nu_texture_t nu_image_create_texture(nu_image_t image);

NU_API nu_model_t nu_model_create(void);
NU_API void       nu_model_delete(nu_model_t model);

NU_API nu_font_t nu_font_create_default(void);
NU_API void      nu_font_delete(nu_font_t font);

#endif
