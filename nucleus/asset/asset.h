#ifndef NU_ASSET_H
#define NU_ASSET_H

#include <nucleus/asset/texture.h>
#include <nucleus/asset/model.h>

NU_DEFINE_HANDLE(nu_asset_handle_t);
NU_DEFINE_HANDLE(nu_asset_bundle_t);

typedef enum
{
    NU_ASSET_TEXTURE,
    NU_ASSET_MATERIAL,
    NU_ASSET_MESH,
    NU_ASSET_MODEL,
    NU_ASSET_FONT,
    NU_ASSET_INPUT,
    NU_ASSET_TYPE_MAX,
} nu_asset_type_t;

typedef struct
{
    const nu_char_t *filename;
} nu_asset_load_info_t;

NU_API void      *nu_asset_add(nu_asset_type_t    type,
                               nu_uid_t           uid,
                               nu_asset_handle_t *handle);
NU_API void      *nu_asset_get(nu_asset_handle_t handle);
NU_API void      *nu_asset_find(nu_asset_type_t    type,
                                nu_uid_t           uid,
                                nu_asset_handle_t *handle);
NU_API nu_bool_t  nu_asset_find_handle(nu_asset_type_t    type,
                                       nu_uid_t           uid,
                                       nu_asset_handle_t *handle);
NU_API nu_error_t nuext_asset_load_filename(nu_asset_type_t    type,
                                            const nu_char_t   *filename,
                                            const nu_char_t   *name,
                                            nu_asset_handle_t *handle);

#define nu_asset_find_texture(name) \
    ((nu_texture_t *)nu_asset_find(NU_ASSET_TEXTURE, NU_UID(name), NU_NULL))
#define nu_asset_find_material(name) \
    ((nu_material_t *)nu_asset_find(NU_ASSET_MATERIAL, NU_UID(name), NU_NULL))
#define nu_asset_find_model(name) \
    ((nu_model_t *)nu_asset_find(NU_ASSET_MODEL, NU_UID(name), NU_NULL))

#endif
