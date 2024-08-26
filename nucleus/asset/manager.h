#ifndef NU_ASSET_MANAGER_H
#define NU_ASSET_MANAGER_H

#include <nucleus/graphics.h>

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

typedef struct
{
    nu_error_t (*load_async)(void *ctx, void *data);
    nu_error_t (*load_sync)(void *ctx, void *data);
    nu_error_t (*unload)(void *ctx, void *data);
} nu_asset_loader_t;

NU_DEFINE_HANDLE(nu_asset_handle_t);

#define NU_DEFINE_ASSET(name, type, desc) \
    typedef struct                        \
    {                                     \
        nu_asset_handle_t handle;         \
        type             *data;           \
    } name

NU_DEFINE_ASSET(nu_asset_model_t, nu_model_t, void);

NU_API nu_error_t nu_asset_manager_init(nu_allocator_t     *alloc,
                                        nu_asset_manager_t *manager);
NU_API void       nu_asset_manager_free(nu_asset_manager_t *manager);

#endif
