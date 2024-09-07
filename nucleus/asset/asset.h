#ifndef NU_ASSET_H
#define NU_ASSET_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/platform/platform.h>

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

NU_API nu_asset_t nu_asset_add(nu_asset_type_t type, nu_uid_t uid);
NU_API nu_asset_t nu_asset_find(nu_asset_type_t type, nu_uid_t uid);

NU_API nu_asset_data_t *nu_asset_data(nu_asset_t handle);
NU_API nu_asset_type_t  nu_asset_type(nu_asset_t handle);
NU_API nu_bundle_t      nu_asset_bundle(nu_asset_t handle);

NU_API nu_asset_t nuext_asset_load_filename(nu_asset_type_t  type,
                                            nu_uid_t         uid,
                                            const nu_char_t *filename);

#endif
