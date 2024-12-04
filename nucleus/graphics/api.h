#ifndef NU_GRAPHICS_API_H
#define NU_GRAPHICS_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_OBJECT(nu_image_t);
NU_DEFINE_OBJECT(nu_geometry_t);
NU_DEFINE_OBJECT(nu_camera_t);
NU_DEFINE_OBJECT(nu_texture_t);
NU_DEFINE_OBJECT(nu_model_t);
NU_DEFINE_OBJECT(nu_font_t);
NU_DEFINE_OBJECT(nu_mesh_t);
NU_DEFINE_OBJECT(nu_material_t);
NU_DEFINE_OBJECT(nu_light_t);
NU_DEFINE_OBJECT(nu_lightenv_t);
NU_DEFINE_OBJECT(nu_renderpass_t);

#define NU_CUBEMAP_FACE_COUNT 6

typedef enum
{
    NU_TEXTURE_COLORMAP,
    NU_TEXTURE_COLORMAP_TARGET,
    NU_TEXTURE_DEPTHBUFFER_TARGET,
    NU_TEXTURE_SHADOWMAP_TARGET,
    NU_TEXTURE_CUBEMAP,
} nu_texture_type_t;

typedef enum
{
    NU_IMAGE_RGBA
} nu_image_type_t;

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

NU_API nu_camera_t nu_camera_new(void);
NU_API void        nu_camera_set_view(nu_camera_t camera, nu_m4_t view);
NU_API void        nu_camera_set_proj(nu_camera_t camera, nu_m4_t proj);

NU_API nu_mesh_t nu_mesh_new(nu_primitive_t primitive, nu_size_t capacity);
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
NU_API size_t    nu_mesh_capacity(nu_mesh_t mesh);

NU_API nu_object_type_t  nu_texture(void);
NU_API nu_texture_t      nu_texture_new(nu_texture_type_t type,
                                        nu_v3u_t          size,
                                        nu_size_t         layer);
NU_API nu_texture_t      nu_texture_new_from_color(nu_color_t color);
NU_API nu_texture_t      nu_texture_new_from_image(nu_texture_type_t type,
                                                   nu_image_t        image);
NU_API nu_texture_t      nu_texture_new_image_texture(nu_texture_type_t type,
                                                      nu_image_t        image);
NU_API void              nu_texture_set_data(nu_texture_t     texture,
                                             nu_size_t        layer,
                                             const nu_byte_t *data);
NU_API nu_texture_type_t nu_texture_type(nu_texture_t texture);
NU_API nu_image_t        nu_texture_image(nu_texture_t texture);

NU_API nu_material_t nu_material_new(nu_material_type_t type);
NU_API nu_material_t nu_material_new_color(nu_material_type_t type,
                                           nu_color_t         color);
NU_API void nu_material_set_color(nu_material_t material, nu_color_t color);
NU_API void nu_material_set_texture(nu_material_t material,
                                    nu_texture_t  texture);
NU_API void nu_material_set_uv_transform(nu_material_t material, nu_m3_t mat);
NU_API void nu_material_set_wrap_mode(nu_material_t          material,
                                      nu_texture_wrap_mode_t mode);

NU_API nu_light_t nu_light_new(nu_light_type_t type);
NU_API void       nu_light_set_position(nu_light_t light, nu_v3_t v);
NU_API void       nu_light_set_rotation(nu_light_t light, nu_q4_t q);
NU_API void       nu_light_set_color(nu_light_t light, nu_color_t v);

NU_API nu_lightenv_t nu_lightenv_new(void);
NU_API void          nu_lightenv_add_light(nu_lightenv_t env, nu_light_t light);
NU_API void          nu_lightenv_add_shadowmap(nu_lightenv_t env,
                                               nu_texture_t  shadowmap,
                                               nu_camera_t   camera);
NU_API void          nu_lightenv_set_skybox(nu_lightenv_t env,
                                            nu_texture_t  cubemap,
                                            nu_q4_t       rotation);

NU_API nu_renderpass_t nu_renderpass_new(nu_renderpass_type_t type);
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

NU_API nu_object_type_t nu_image(void);
NU_API nu_image_t       nu_image_new(nu_image_type_t type,
                                     nu_v3u_t        size,
                                     nu_size_t       layer);
NU_API nu_byte_t       *nu_image_data(nu_image_t image, nu_size_t layer);
NU_API nu_v3u_t         nu_image_size(nu_image_t image);
NU_API nu_image_t       nu_image_load(nu_seria_t seria);
NU_API void             nu_image_save(nu_image_t image, nu_seria_t seria);

NU_API nu_geometry_t nu_geometry_new_mesh(nu_primitive_t primitive,
                                          nu_size_t      position_capacity,
                                          nu_size_t      uv_capacity,
                                          nu_size_t      vertex_capacity);
NU_API void          nu_geometry_clear(nu_geometry_t geometry);
NU_API void          nu_geometry_cube(nu_geometry_t geometry, nu_f32_t unit);
NU_API void          nu_geometry_plane(nu_geometry_t geometry,
                                       nu_f32_t      width,
                                       nu_f32_t      height);
NU_API void          nu_geometry_grid(nu_geometry_t geometry,
                                      nu_u32_t      width,
                                      nu_u32_t      height,
                                      nu_f32_t      unit,
                                      nu_f32_t      uv_scale);
NU_API void          nu_geometry_transform(nu_geometry_t geometry, nu_m4_t m);
NU_API void          nu_geometry_merge(nu_geometry_t dst, nu_geometry_t src);
NU_API nu_b3_t       nu_geometry_bounds(nu_geometry_t geometry);
NU_API nu_mesh_t     nu_mesh_new_geometry(nu_geometry_t geometry);
NU_API nu_mesh_t     nu_mesh_new_geometry_normals(nu_geometry_t geometry);
#ifdef NU_BUILD_UTILS_SERIA
NU_API void nu_geometry_load(nu_geometry_t geometry, nu_seria_t seria);
NU_API void nu_geometry_save(nu_geometry_t geometry, nu_seria_t seria);
#endif

NU_API nu_object_type_t nu_model(void);
NU_API nu_model_t       nu_model_new(nu_size_t node_count);
NU_API void             nu_model_set(nu_model_t    model,
                                     nu_size_t     index,
                                     nu_mesh_t     mesh,
                                     nu_material_t material,
                                     nu_m4_t       transform);
NU_API nu_model_t       nu_model_load(nu_seria_t seria);
NU_API void             nu_model_save(nu_model_t model, nu_seria_t seria);

NU_API nu_font_t nu_font_new_default(void);

#endif
