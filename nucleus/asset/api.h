#ifndef NU_ASSET_API_H
#define NU_ASSET_API_H

#include <nucleus/graphics/api.h>

#define NU_ASSET_NAME_MAX 32

NU_DEFINE_HANDLE(nu_asset_t);
NU_DEFINE_HANDLE(nu_bundle_t);

typedef enum
{
    NU_ASSET_TEXTURE,
    NU_ASSET_CUBEMAP,
    NU_ASSET_MATERIAL,
    NU_ASSET_MODEL,
    NU_ASSET_INPUT,
    NU_ASSET_TABLE,
    NU_ASSET_UNKNOWN,
} nu_asset_type_t;

typedef struct
{
    const nu_char_t *name;
    nu_asset_type_t  type;
    nu_bundle_t      bundle;
} nu_asset_info_t;

NU_API nu_asset_t nu_asset_add(nu_asset_type_t type, const nu_char_t *name);
NU_API nu_asset_t nu_asset_find(nu_asset_type_t type, const nu_char_t *name);
NU_API nu_bool_t  nu_asset_exists(nu_asset_type_t type, const nu_char_t *name);
NU_API void      *nu_asset_data(nu_asset_t handle);
NU_API nu_asset_info_t nu_asset_info(nu_asset_t handle);

#define nu_asset_texture(name) \
    ((nu_texture_t)nu_asset_data(nu_asset_find(NU_ASSET_TEXTURE, name)))
#define nu_asset_cubemap(name) \
    ((nu_cubemap_t)nu_asset_data(nu_asset_find(NU_ASSET_CUBEMAP, name)))
#define nu_asset_model(name) \
    ((nu_model_t)nu_asset_data(nu_asset_find(NU_ASSET_MODEL, name)))
#define nu_asset_table(name) \
    ((nu_table_t)nu_asset_data(nu_asset_find(NU_ASSET_TABLE, name)))

#endif
