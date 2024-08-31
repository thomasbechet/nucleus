#ifndef NU_ASSET_MANAGER_IMPL_H
#define NU_ASSET_MANAGER_IMPL_H

#include <nucleus/asset/manager.h>
#include <nucleus/asset/texture.h>

nu_error_t
nu_asset_manager_create (nu_allocator_t alloc, nu_asset_manager_t *manager)
{
    nu_error_t error;

    manager->_ptr
        = (nu__asset_manager_t *)nu_alloc(alloc, sizeof(*manager->_ptr));
    manager->_ptr->allocator = alloc;
    nu_pool_init(&manager->_ptr->entries, alloc, 10);

    for (nu_size_t i = 0; i < NU_ASSET_TYPE_MAX; ++i)
    {
        manager->_ptr->types[i].size          = 0;
        manager->_ptr->types[i].loader.loader = NU_NULL;
    }

    return NU_ERROR_NONE;
}
void
nu_asset_manager_delete (nu_asset_manager_t manager)
{
    // Free base loaders
    if (manager._ptr->types[NU_ASSET_TEXTURE].loader.loader)
    {
        nu_texture_loader_free(&manager._ptr->types[NU_ASSET_TEXTURE].loader,
                               manager._ptr->allocator);
    }
    nu_pool_free(&manager._ptr->entries, manager._ptr->allocator);
    nu_free(manager._ptr->allocator, manager._ptr, sizeof(*manager._ptr));
}
nu_error_t
nu_asset_register_base_loaders (nu_asset_manager_t manager,
                                nu_platform_t      platform,
                                nu_renderer_t      renderer)
{
    nu_error_t error;
    error
        = nu_texture_loader_init(manager._ptr->allocator,
                                 renderer,
                                 &manager._ptr->types[NU_ASSET_TEXTURE].loader);
    NU_ERROR_CHECK(error, return error);
    return NU_ERROR_NONE;
}

void *
nu_asset_add (nu_asset_manager_t manager,
              nu_asset_type_t    type,
              nu_uid_t           uid,
              nu_asset_handle_t *handle)
{
    nu__asset_type_t *t = manager._ptr->types + type;

    nu_u32_t           index;
    nu__asset_entry_t *entry
        = nu_pool_add(&manager._ptr->entries, manager._ptr->allocator, &index);
    entry->type = type;
    entry->uid  = uid;
    entry->data = nu_alloc(manager._ptr->allocator, t->size);
    entry->used = NU_TRUE;
    nu_memset(entry->data, 0, t->size);

    if (handle)
    {
        handle->_index = index;
    }

    return entry->data;
}
void *
nu_asset_get (nu_asset_manager_t manager, nu_asset_handle_t handle)
{
    return manager._ptr->entries.data + handle._index;
}
void *
nu_asset_find (nu_asset_manager_t manager,
               nu_asset_type_t    type,
               nu_uid_t           uid,
               nu_asset_handle_t *handle)
{
    nu_asset_handle_t h;
    if (nu_asset_find_handle(manager, type, uid, &h))
    {
        if (handle)
        {
            *handle = h;
        }
        return manager._ptr->entries.data[h._index].data;
    }
    return NU_NULL;
}
nu_bool_t
nu_asset_find_handle (nu_asset_manager_t manager,
                      nu_asset_type_t    type,
                      nu_uid_t           uid,
                      nu_asset_handle_t *handle)
{
    NU_ASSERT(handle);
    for (nu_size_t i = 0; i < manager._ptr->entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = manager._ptr->entries.data + i;
        if (entry->used && entry->type == type && entry->uid == uid)
        {
            handle->_index = i;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
nu_error_t
nuext_asset_load_filename (nu_asset_manager_t manager,
                           nu_asset_type_t    type,
                           const nu_char_t   *filename,
                           const nu_char_t   *name,
                           nu_asset_handle_t *handle)
{
    void             *data = nu_asset_add(manager, type, NU_UID(name), handle);
    nu__asset_type_t *t    = manager._ptr->types + type;

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
