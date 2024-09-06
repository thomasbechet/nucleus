#ifndef NU_GRAPHICS_INTERNAL_H
#define NU_GRAPHICS_INTERNAL_H

#include <nucleus/graphics/renderer.h>

//////////////////////////////////////////////////////////////////////////
//////                            Font                              //////
//////////////////////////////////////////////////////////////////////////

#include <nucleus/graphics/font_data.h>

typedef struct
{
    nu_texture_t  texture;
    nu_material_t material;
    nu_rect_t    *glyphs;
    nu_size_t     glyphs_count;
    nu_uvec2_t    glyph_size;
    nu_char_t     min_char;
    nu_char_t     max_char;
} nu__font_t;

typedef nu_pool(nu__font_t) nu__font_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                            Image                             //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_uvec2_t     size;
    nu_color_t    *colors;
    nu_allocator_t allocator;
} nu__image_t;

typedef nu_pool(nu__image_t) nu__image_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                            Model                             //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu_u16_t  mesh;
    nu_u16_t  material;
    nu_mat4_t transform;
} nu__model_node_t;

typedef union
{
    nu_mesh_t     mesh;
    nu_texture_t  texture;
    nu_material_t material;
} nu__model_asset_t;

typedef nu_vec(nu__model_asset_t) nu__model_asset_vec_t;
typedef nu_vec(nu__model_node_t) nu__model_node_vec_t;

typedef struct
{
    nu_allocator_t        allocator;
    nu__model_asset_vec_t assets;
    nu__model_node_vec_t  nodes;
} nu__model_t;

typedef nu_pool(nu__model_t) nu__model_pool_t;

//////////////////////////////////////////////////////////////////////////
//////                          Renderer                            //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    // Engine API
    nu_error_t (*init)(void);
    nu_error_t (*free)(void);
    nu_error_t (*render)(const nu_rect_t *global_viewport,
                         const nu_rect_t *viewport);
    nu_texture_t (*create_surface_color)(nu_uvec2_t size);

    // Resources API
    nu_camera_t (*create_camera)(const nu_camera_info_t *info);
    nu_error_t (*delete_camera)(nu_camera_t camera);
    nu_error_t (*update_camera)(nu_camera_t             camera,
                                const nu_camera_info_t *info);

    nu_mesh_t (*create_mesh)(const nu_mesh_info_t *info);
    nu_error_t (*delete_mesh)(nu_mesh_t mesh);

    nu_texture_t (*create_texture)(const nu_texture_info_t *info);
    nu_error_t (*delete_texture)(nu_texture_t texture);

    nu_cubemap_t (*create_cubemap)(const nu_cubemap_info_t *info);
    nu_error_t (*delete_cubemap)(nu_cubemap_t cubemap);

    nu_material_t (*create_material)(const nu_material_info_t *info);
    nu_error_t (*delete_material)(nu_material_t material);
    nu_error_t (*update_material)(nu_material_t             material,
                                  const nu_material_info_t *info);

    nu_renderpass_t (*create_renderpass)(const nu_renderpass_info_t *info);
    nu_error_t (*delete_renderpass)(nu_renderpass_t pass);

    // Commands API
    void (*submit_renderpass)(nu_renderpass_t               pass,
                              const nu_renderpass_submit_t *info);
    void (*reset_renderpass)(nu_renderpass_t pass);
    void (*draw_mesh)(nu_renderpass_t pass,
                      nu_material_t   material,
                      nu_mesh_t       mesh,
                      nu_mat4_t       transform);
    void (*draw_blit)(nu_renderpass_t pass,
                      nu_material_t   material,
                      nu_rect_t       extent,
                      nu_rect_t       tex_extent);
} nu_renderer_api_t;

typedef struct
{
    nu_renderer_api_t api;
    nu_bool_t         null_api;
    nu_texture_t      surface_color;
} nu__renderer_t;

//////////////////////////////////////////////////////////////////////////
//////                          Module                              //////
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    nu__renderer_t   renderer;
    nu__font_pool_t  fonts;
    nu__image_pool_t images;
    nu__model_pool_t models;
} nu__graphics_t;

static nu_error_t nu__graphics_init(const nu_renderer_info_t *info);
static nu_error_t nu__graphics_free(void);

#endif
