#ifndef NU_ASSET_MANAGER_IMPL_H
#define NU_ASSET_MANAGER_IMPL_H

#include <nucleus/asset/manager.h>
#include <nucleus/asset/texture.h>

nu_error_t
nu_asset_manager_init (nu_allocator_t *alloc, nu_asset_manager_t *manager)
{
    nu_error_t error;

    manager->_allocator = alloc;
    nu_pool_init(&manager->_entries, alloc, 10);

    for (nu_size_t i = 0; i < NU_ASSET_TYPE_MAX; ++i)
    {
        manager->_types[i].size          = 0;
        manager->_types[i].loader.loader = NU_NULL;
    }

    return NU_ERROR_NONE;
}
void
nu_asset_manager_free (nu_asset_manager_t *manager)
{
    // Free base loaders
    if (manager->_types[NU_ASSET_TEXTURE].loader.loader)
    {
        nu_texture_loader_free(&manager->_types[NU_ASSET_TEXTURE].loader,
                               manager->_allocator);
    }
    nu_pool_free(&manager->_entries, manager->_allocator);
}
nu_error_t
nu_asset_register_base_loaders (nu_asset_manager_t *manager,
                                nu_platform_t      *platform,
                                nu_renderer_t       renderer)
{
    nu_error_t error;
    error = nu_texture_loader_init(manager->_allocator,
                                   renderer,
                                   &manager->_types[NU_ASSET_TEXTURE].loader);
    NU_ERROR_CHECK(error, return error);
    return NU_ERROR_NONE;
}

void *
nu_asset_add (nu_asset_manager_t *manager,
              nu_asset_type_t     type,
              nu_uid_t            uid,
              nu_asset_handle_t  *handle)
{
    nu__asset_type_t *t = manager->_types + type;

    nu_u32_t           index;
    nu__asset_entry_t *entry
        = nu_pool_add(&manager->_entries, manager->_allocator, &index);
    entry->type = type;
    entry->uid  = uid;
    entry->data = nu_alloc(manager->_allocator, t->size);
    entry->used = NU_TRUE;
    nu_memset(entry->data, 0, t->size);

    if (handle)
    {
        handle->index = index;
    }

    return entry->data;
}
void *
nu_asset_get (nu_asset_manager_t *manager, nu_asset_handle_t handle)
{
    return manager->_entries.data + handle.index;
}
void *
nu_asset_find (nu_asset_manager_t *manager,
               nu_asset_type_t     type,
               nu_uid_t            uid,
               nu_asset_handle_t  *handle)
{
    nu_asset_handle_t h;
    if (nu_asset_find_handle(manager, type, uid, &h))
    {
        if (handle)
        {
            *handle = h;
        }
        return manager->_entries.data[h.index].data;
    }
    return NU_NULL;
}
nu_bool_t
nu_asset_find_handle (const nu_asset_manager_t *manager,
                      nu_asset_type_t           type,
                      nu_uid_t                  uid,
                      nu_asset_handle_t        *handle)
{
    NU_ASSERT(handle);
    for (nu_size_t i = 0; i < manager->_entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = manager->_entries.data + i;
        if (entry->used && entry->type == type && entry->uid == uid)
        {
            handle->index = i;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
nu_error_t
nuext_asset_load_filename (nu_asset_manager_t *manager,
                           nu_asset_type_t     type,
                           const nu_char_t    *filename,
                           const nu_char_t    *name,
                           nu_asset_handle_t  *handle)
{
    void             *data = nu_asset_add(manager, type, NU_UID(name), handle);
    nu__asset_type_t *t    = manager->_types + type;

    nu_error_t error = NU_ERROR_NONE;
    if (t->loader.loader)
    {
        nu_asset_load_info_t info = { .filename = filename };
        error = t->loader.load_async(t->loader.loader, &info, data);
        NU_ERROR_CHECK(error, return error);
        error = t->loader.load_sync(t->loader.loader, &info, data);
        NU_ERROR_CHECK(error, return error);
    }

    return error;
}

#endif
