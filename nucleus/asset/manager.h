#ifndef NU_ASSET_MANAGER_H
#define NU_ASSET_MANAGER_H

#include <nucleus/core.h>

NU_DEFINE_HANDLE(nu_asset_handle_t);
NU_DEFINE_HANDLE(nu_asset_bundle_t);

typedef enum
{
    NU_ASSET_TEXTURE,
    NU_ASSET_MATERIAL,
    NU_ASSET_MESH,
    NU_ASSET_MODEL,
    NU_ASSET_FONT,
    NU_ASSET_TYPE_MAX,
} nu_asset_type_t;

typedef struct
{
    const nu_char_t *filename;
} nu_asset_load_info_t;

typedef struct
{
    void *loader;
    nu_error_t (*load_async)(void                       *loader,
                             const nu_asset_load_info_t *info,
                             void                       *data);
    nu_error_t (*load_sync)(void                       *loader,
                            const nu_asset_load_info_t *info,
                            void                       *data);
    nu_error_t (*unload)(void *loader, void *data);
} nu_asset_loader_t;

typedef struct
{
    nu_size_t         size;
    nu_asset_loader_t loader;
} nu__asset_type_t;

typedef struct
{
    nu_asset_type_t   type;
    nu_uid_t          uid;
    void             *data;
    nu_asset_bundle_t bundle;
} nu__asset_entry_t;

typedef nu_slotmap(nu__asset_entry_t) nu__asset_entry_slotmap_t;

typedef struct
{
    nu_allocator_t           *_allocator;
    nu__asset_type_t          _types[NU_ASSET_TYPE_MAX];
    nu__asset_entry_slotmap_t _entries;
    nu_asset_bundle_t         _active_bundle;
} nu_asset_manager_t;

NU_API nu_error_t nu_asset_manager_init(nu_allocator_t     *alloc,
                                        nu_asset_manager_t *manager);
NU_API void       nu_asset_manager_free(nu_asset_manager_t *manager);

NU_API void      *nu_asset_add(nu_asset_manager_t *manager,
                               nu_asset_type_t     type,
                               nu_uid_t            uid,
                               nu_asset_handle_t  *handle);
NU_API void      *nu_asset_get(nu_asset_manager_t *manager,
                               nu_asset_handle_t   handle);
NU_API void      *nu_asset_find(nu_asset_manager_t *manager, nu_uid_t uid);
NU_API nu_bool_t  nu_asset_find_handle(const nu_asset_manager_t *manager,
                                       nu_uid_t                  uid,
                                       nu_asset_handle_t        *handle);
NU_API nu_error_t nu_asset_load_filename(nu_asset_manager_t *manager,
                                         nu_asset_type_t     type,
                                         const nu_char_t    *filename,
                                         nu_asset_handle_t  *handle);

#endif
