#ifndef NU_ASSET_API_H
#define NU_ASSET_API_H

#include <nucleus/graphics/api.h>

NU_DEFINE_HANDLE(nu_asset_t);
NU_DEFINE_HANDLE(nu_bundle_t);

#define NU_ASSET_NAME_MAX 32

#define NU_ASSET_TEXTURE(name) \
    ((nu_texture_t)nu_asset_data(nu_asset_find(NU_ASSET_TEXTURE, name)))
#define NU_ASSET_MODEL(name) \
    ((nu_model_t)nu_asset_data(nu_asset_find(NU_ASSET_MODEL, name)))

typedef enum
{
    NU_ASSET_TEXTURE,
    NU_ASSET_MATERIAL,
    NU_ASSET_MODEL,
    NU_ASSET_INPUT,
    NU_ASSET_TABLE,
    NU_ASSET_UNKNOWN,
} nu_asset_type_t;

typedef struct
{
    nu_byte_t       name[NU_ASSET_NAME_MAX];
    nu_asset_type_t type;
    nu_bundle_t     bundle;
} nu_asset_info_t;

NU_API nu_asset_t      nu_asset_add(nu_asset_type_t type, nu_str_t name);
NU_API nu_asset_t      nu_asset_find(nu_asset_type_t type, nu_str_t name);
NU_API nu_bool_t       nu_asset_exists(nu_asset_type_t type, nu_str_t name);
NU_API void           *nu_asset_data(nu_asset_t handle);
NU_API nu_asset_info_t nu_asset_info(nu_asset_t handle);

#endif
