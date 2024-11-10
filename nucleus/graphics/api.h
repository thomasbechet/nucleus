#ifndef NU_GRAPHICS_API_H
#define NU_GRAPHICS_API_H

#include <nucleus/seria/api.h>
#ifdef NU_BUILD_RESOURCE
#include <nucleus/resource/api.h>
#endif

NU_DEFINE_HANDLE(nu_image_t);
NU_DEFINE_HANDLE(nu_camera_t);
NU_DEFINE_HANDLE(nu_texture_t);
NU_DEFINE_HANDLE(nu_model_t);
NU_DEFINE_HANDLE(nu_font_t);
NU_DEFINE_HANDLE(nu_mesh_t);
NU_DEFINE_HANDLE(nu_material_t);
NU_DEFINE_HANDLE(nu_light_t);
NU_DEFINE_HANDLE(nu_lightenv_t);
NU_DEFINE_HANDLE(nu_renderpass_t);

#define NU_CUBEMAP_FACE_COUNT 6

typedef enum
{
    NU_TEXTURE_COLOR,
    NU_TEXTURE_COLOR_TARGET,
    NU_TEXTURE_DEPTH_TARGET,
    NU_TEXTURE_SHADOW_TARGET,
    NU_TEXTURE_CUBEMAP_COLOR,
} nu_texture_type_t;

typedef enum
{
    NU_TEXTURE_WRAP_CLAMP,
    NU_TEXTURE_WRAP_REPEAT,
    NU_TEXTURE_WRAP_MIRROR
} nu_texture_wrap_mode_t;

typedef enum
{
    NU_MATERIAL_SURFACE,
    NU_MATERIAL_CANVAS,
} nu_material_type_t;

typedef enum
{
    NU_LIGHT_DIRECTIONAL,
    NU_LIGHT_POINT,
} nu_light_type_t;

typedef enum
{
    NU_PRIMITIVE_POINTS,
    NU_PRIMITIVE_LINES,
    NU_PRIMITIVE_LINES_STRIP,
    NU_PRIMITIVE_TRIANGLES,
} nu_primitive_t;

typedef enum
{
    NU_RENDERPASS_FORWARD,
    NU_RENDERPASS_CANVAS,
    NU_RENDERPASS_SHADOW
} nu_renderpass_type_t;

typedef enum
{
    NU_SHADE_UNLIT,
    NU_SHADE_LIT,
    NU_SHADE_WIREFRAME,
} nu_shademode_t;

typedef enum
{
    NU_CUBEMAP_POSX = 0,
    NU_CUBEMAP_NEGX = 1,
    NU_CUBEMAP_POSY = 2,
    NU_CUBEMAP_NEGY = 3,
    NU_CUBEMAP_POSZ = 4,
    NU_CUBEMAP_NEGZ = 5
} nu_cubemap_face_t;

// Backend api
NU_API nu_texture_t nu_surface_color_target(void);

NU_API nu_camera_t nu_camera_create(void);
NU_API void        nu_camera_delete(nu_camera_t camera);
NU_API void        nu_camera_set_view(nu_camera_t camera, nu_m4_t view);
NU_API void        nu_camera_set_proj(nu_camera_t camera, nu_m4_t proj);

NU_API nu_mesh_t nu_mesh_create(nu_primitive_t primitive, nu_size_t capacity);
NU_API void      nu_mesh_delete(nu_mesh_t mesh);
NU_API void      nu_mesh_set_uvs(nu_mesh_t      mesh,
                                 nu_size_t      first,
                                 nu_size_t      count,
                                 const nu_v2_t *data);
NU_API void      nu_mesh_set_positions(nu_mesh_t      mesh,
                                       nu_size_t      first,
                                       nu_size_t      count,
                                       const nu_v3_t *data);
NU_API void      nu_mesh_set_colors(nu_mesh_t         mesh,
                                    nu_size_t         first,
                                    nu_size_t         count,
                                    const nu_color_t *data);

NU_API nu_texture_t nu_texture_create(nu_texture_type_t type, nu_v3u_t size);
NU_API nu_texture_t nu_texture_create_color(nu_color_t color);
NU_API void         nu_texture_delete(nu_texture_t texture);
NU_API void         nu_texture_set_colors(nu_texture_t      texture,
                                          const nu_color_t *colors);
NU_API void         nu_texture_set_cubemap_colors(nu_texture_t      cubemap,
                                                  nu_cubemap_face_t face,
                                                  const nu_color_t *colors);

NU_API nu_material_t nu_material_create(nu_material_type_t type);
NU_API nu_material_t nu_material_create_color(nu_material_type_t type,
                                              nu_color_t         color);
NU_API void          nu_material_delete(nu_material_t material);
NU_API void nu_material_set_color(nu_material_t material, nu_color_t color);
NU_API void nu_material_set_texture(nu_material_t material,
                                    nu_texture_t  texture);
NU_API void nu_material_set_uv_transform(nu_material_t material, nu_m3_t mat);
NU_API void nu_material_set_wrap_mode(nu_material_t          material,
                                      nu_texture_wrap_mode_t mode);

NU_API nu_light_t nu_light_create(nu_light_type_t type);
NU_API void       nu_light_delete(nu_light_t light);
NU_API void       nu_light_set_position(nu_light_t light, nu_v3_t v);
NU_API void       nu_light_set_rotation(nu_light_t light, nu_q4_t q);
NU_API void       nu_light_set_color(nu_light_t light, nu_color_t v);

NU_API nu_lightenv_t nu_lightenv_create(void);
NU_API void          nu_lightenv_delete(nu_lightenv_t env);
NU_API void          nu_lightenv_add_light(nu_lightenv_t env, nu_light_t light);
NU_API void          nu_lightenv_add_shadowmap(nu_lightenv_t env,
                                               nu_texture_t  shadowmap,
                                               nu_camera_t   camera);
NU_API void          nu_lightenv_set_skybox(nu_lightenv_t env,
                                            nu_texture_t  cubemap,
                                            nu_q4_t       rotation);

NU_API nu_renderpass_t nu_renderpass_create(nu_renderpass_type_t type);
NU_API void            nu_renderpass_delete(nu_renderpass_t pass);
NU_API void            nu_renderpass_reset(nu_renderpass_t pass);
NU_API void            nu_renderpass_submit(nu_renderpass_t pass);

NU_API void nu_renderpass_set_reset_after_submit(nu_renderpass_t pass,
                                                 nu_bool_t bool);
NU_API void nu_renderpass_set_clear_color(nu_renderpass_t pass,
                                          nu_color_t     *color);
NU_API void nu_renderpass_set_camera(nu_renderpass_t pass, nu_camera_t camera);
NU_API void nu_renderpass_set_color_target(nu_renderpass_t pass,
                                           nu_texture_t    texture);
NU_API void nu_renderpass_set_depth_target(nu_renderpass_t pass,
                                           nu_texture_t    texture);
NU_API void nu_renderpass_set_shade(nu_renderpass_t pass, nu_shademode_t mode);
NU_API void nu_renderpass_set_lightenv(nu_renderpass_t pass, nu_lightenv_t env);

NU_API void nu_draw_submesh_instanced(nu_renderpass_t pass,
                                      nu_mesh_t       mesh,
                                      nu_size_t       first,
                                      nu_size_t       count,
                                      nu_material_t   material,
                                      const nu_m4_t  *transforms,
                                      nu_size_t       instance_count);
NU_API void nu_draw_blit(nu_renderpass_t pass,
                         nu_b2i_t        extent,
                         nu_b2i_t        tex_extent,
                         nu_material_t   material);
NU_API void nu_draw_blit_sliced(nu_renderpass_t pass,
                                nu_b2i_t        extent,
                                nu_b2i_t        tex_extent,
                                nu_b2i_t        inner,
                                nu_material_t   material);
NU_API void nu_draw_mesh(nu_renderpass_t pass,
                         nu_mesh_t       mesh,
                         nu_material_t   material,
                         nu_m4_t         transform);
NU_API void nu_draw_submesh(nu_renderpass_t pass,
                            nu_mesh_t       mesh,
                            nu_size_t       first,
                            nu_size_t       count,
                            nu_material_t   material,
                            nu_m4_t         transform);
NU_API void nu_draw_mesh_instanced(nu_renderpass_t pass,
                                   nu_mesh_t       mesh,
                                   nu_material_t   material,
                                   const nu_m4_t  *transforms,
                                   nu_size_t       instance_count);
NU_API void nu_draw_model(nu_renderpass_t pass,
                          nu_model_t      model,
                          nu_m4_t         transform);
NU_API void nu_draw_text(nu_renderpass_t pass,
                         nu_str_t        str,
                         nu_font_t       font,
                         nu_v2i_t        pos);
NU_API void nu_draw_points(nu_renderpass_t pass,
                           const nu_v3_t  *positions,
                           nu_size_t       count,
                           nu_material_t   material,
                           nu_m4_t         transform);
NU_API void nu_draw_lines(nu_renderpass_t pass,
                          const nu_v3_t  *positions,
                          nu_size_t       count,
                          nu_material_t   material,
                          nu_m4_t         transform);
NU_API void nu_draw_lines_strip(nu_renderpass_t pass,
                                const nu_v3_t  *positions,
                                nu_size_t       count,
                                nu_material_t   material,
                                nu_m4_t         transform);
NU_API void nu_draw_triangles(nu_renderpass_t pass,
                              const nu_v3_t  *positions,
                              const nu_v2_t  *uvs,
                              nu_size_t       count,
                              nu_material_t   material,
                              nu_m4_t         transform);
NU_API void nu_draw_box(nu_renderpass_t pass,
                        nu_b3_t         box,
                        nu_material_t   material,
                        nu_m4_t         transform);

NU_API nu_image_t   nu_image_create(nu_v2u_t size);
NU_API nu_texture_t nu_image_create_texture(nu_image_t image);
NU_API void         nu_image_delete(nu_image_t image);
NU_API nu_color_t  *nu_image_colors(nu_image_t image);
NU_API nu_v2u_t     nu_image_size(nu_image_t image);
NU_API nu_image_t   nu_image_load(nu_seria_t seria);
NU_API void         nu_image_save(nu_image_t image, nu_seria_t seria);

NU_API nu_model_t nu_model_create(void);
NU_API void       nu_model_delete(nu_model_t model);

NU_API nu_font_t nu_font_create_default(void);
NU_API void      nu_font_delete(nu_font_t font);

#ifdef NU_BUILD_RESOURCE
NU_API nu_image_t   nu_image_resource(nu_uid_t uid);
NU_API nu_texture_t nu_image_texture_resource(nu_uid_t uid);
NU_API nu_model_t   nu_model_resource(nu_uid_t uid);
#endif

#endif
