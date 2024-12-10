#ifndef NU_GRAPHICS_API_H
#define NU_GRAPHICS_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_OBJECT(nugfx_texture_t);
NU_DEFINE_OBJECT(nugfx_mesh_t);
NU_DEFINE_OBJECT(nugfx_model_t);
NU_DEFINE_OBJECT(nugfx_font_t);

typedef enum
{
    NUGFX_CLAMP,
    NUGFX_REPEAT,
    NUGFX_MIRROR
} nugfx_wrap_t;

typedef enum
{
    NUGFX_POINTS,
    NUGFX_LINES,
    NUGFX_LINES_STRIP,
    NUGFX_TRIANGLES,
} nugfx_primitive_t;

typedef enum
{
    NUGFX_SHADE_UNLIT,
    NUGFX_SHADE_LIT,
    NUGFX_SHADE_WIREFRAME,
} nugfx_shademode_t;

typedef enum
{
    NUGFX_UV       = 1,
    NUGFX_COLOR    = 2,
    NUGFX_POSITION = 4
} nugfx_attribute_t;

// resource api
NU_API nugfx_mesh_t nugfx_new_mesh(nu_size_t         capacity,
                                   nugfx_attribute_t attributes);
NU_API void         nugfx_update_positions(nugfx_mesh_t   mesh,
                                           nu_size_t      first,
                                           nu_size_t      count,
                                           const nu_v3_t *data);
NU_API void         nugfx_update_uvs(nugfx_mesh_t   mesh,
                                     nu_size_t      first,
                                     nu_size_t      count,
                                     const nu_v2_t *data);

NU_API nugfx_texture_t nugfx_new_texture(nu_v3u_t size);
NU_API void            nugfx_update_texture(nugfx_texture_t  texture,
                                            const nu_byte_t *data);

NU_API nugfx_model_t nugfx_new_model(nu_size_t node_count);
NU_API void          nugfx_update_node(nugfx_model_t model,
                                       nu_size_t     index,
                                       nugfx_mesh_t  mesh,
                                       nu_m4_t       transform);

NU_API nugfx_font_t nugfx_default_font(void);

// state api
NU_API void nugfx_push_shademode(nugfx_shademode_t mode);
NU_API void nugfx_push_texture(nugfx_texture_t texture);
NU_API void nugfx_push_font(nugfx_font_t font);
NU_API void nugfx_push_transform(nu_m4_t transform);

// commands api
NU_API void nugfx_draw(nugfx_mesh_t mesh, nu_u16_t first, nu_u16_t count);
NU_API void nugfx_blit(nu_b2i_t dst, nu_b2i_t src);
NU_API void nugfx_clear(nu_color_t color);

// derived commands
NU_API void nugfx_blit_sliced(nu_b2i_t dst, nu_b2i_t src, nu_b2i_t inner);
NU_API void nugfx_draw_model(nugfx_model_t model);
NU_API void nugfx_print(nu_str_t text, nu_v2i_t pos);
NU_API void nugfx_draw_points(const nu_v3_t *positions, nu_size_t count);
NU_API void nugfx_draw_lines(const nu_v3_t *positions, nu_size_t count);
NU_API void nugfx_draw_lines_strip(const nu_v3_t *positions, nu_size_t count);
NU_API void nugfx_draw_triangles(const nu_v3_t *positions,
                                 const nu_v2_t *uvs,
                                 nu_size_t      count);
NU_API void nugfx_draw_box(nu_b3_t box);

#endif
