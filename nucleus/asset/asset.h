#ifndef NU_ASSET_H
#define NU_ASSET_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/platform/platform.h>

#define NU_ASSET_NAME_SIZE 32

NU_DEFINE_HANDLE(nu_asset_t);
NU_DEFINE_HANDLE(nu_bundle_t);

typedef enum
{
    NU_ASSET_TEXTURE,
    NU_ASSET_MATERIAL,
    NU_ASSET_MESH,
    NU_ASSET_MODEL,
    NU_ASSET_FONT,
    NU_ASSET_INPUT,
    NU_ASSET_CUSTOM,
} nu_asset_type_t;

typedef union
{
    nu_texture_t  texture;
    nu_material_t material;
    nu_mesh_t     mesh;
    nu_model_t    model;
    nu_font_t     font;
    nu_input_t    input;
    void         *custom;
} nu_asset_data_t;

typedef struct
{
    const nu_char_t *name;
    nu_asset_type_t  type;
    nu_bundle_t      bundle;
} nu_asset_info_t;

NU_API nu_asset_t nu_asset_add(nu_asset_type_t type, const nu_char_t *name);
NU_API nu_asset_t nu_asset_find(nu_asset_type_t type, const nu_char_t *name);
NU_API nu_asset_data_t nu_asset_data(nu_asset_t handle);
NU_API nu_asset_info_t nu_asset_info(nu_asset_t handle);

#define nu_asset_texture(name) \
    (nu_asset_data(nu_asset_find(NU_ASSET_TEXTURE, name)).texture)
#define nu_asset_model(name) \
    (nu_asset_data(nu_asset_find(NU_ASSET_MODEL, name)).model)

NU_API nu_asset_t nuext_asset_load_filename(nu_asset_type_t  type,
                                            const nu_char_t *name,
                                            const nu_char_t *filename);

#endif
