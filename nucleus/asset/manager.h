#ifndef NU_ASSET_MANAGER_H
#define NU_ASSET_MANAGER_H

#include <nucleus/core.h>

typedef enum
{
    NU_ASSET_TEXTURE,
    NU_ASSET_MATERIAL,
    NU_ASSET_MESH,
    NU_ASSET_MODEL,
    NU_ASSET_FONT,
} nu_asset_type_t;

typedef struct
{
    nu_allocator_t *_allocator;
} nu_asset_manager_t;

NU_DEFINE_HANDLE(nu_asset_handle_t);

NU_API nu_error_t nu_asset_manager_init(nu_allocator_t     *alloc,
                                        nu_asset_manager_t *manager);
NU_API void       nu_asset_manager_free(nu_asset_manager_t *manager);

#endif
