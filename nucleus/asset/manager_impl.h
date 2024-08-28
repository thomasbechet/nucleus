#ifndef NU_ASSET_MANAGER_IMPL_H
#define NU_ASSET_MANAGER_IMPL_H

#include <nucleus/asset/manager.h>

nu_error_t
nu_asset_manager_init (nu_allocator_t *alloc, nu_asset_manager_t *manager)
{
    manager->_allocator = alloc;
    nu_slotmap_init(&manager->_entries, alloc, 10);

    for (nu_size_t i = 0; i < NU_ASSET_TYPE_MAX; ++i)
    {
        manager->_types[i].size = 0;
    }

    return NU_ERROR_NONE;
}
void
nu_asset_manager_free (nu_asset_manager_t *manager)
{
    nu_slotmap_free(&manager->_entries, manager->_allocator);
}

void *
nu_asset_add (nu_asset_manager_t *manager,
              nu_asset_type_t     type,
              nu_uid_t            uid,
              nu_asset_handle_t  *handle)
{
    nu__asset_type_t *t = manager->_types + type;

    nu_slot_t slot;
    nu_slotmap_add(&manager->_entries, manager->_allocator, &slot);
    nu__asset_entry_t *entry = nu_slotmap_get(&manager->_entries, slot);
    entry->type              = type;
    entry->uid               = uid;
    entry->data              = nu_alloc(manager->_allocator, t->size);
    nu_memset(entry->data, 0, t->size);

    if (handle)
    {
        handle->id = slot;
    }

    return entry->data;
}
void *
nu_asset_get (nu_asset_manager_t *manager, nu_asset_handle_t handle)
{
    nu_slot_t slot = handle.id;
    return nu_slotmap_get(&manager->_entries, slot)->data;
}
void *
nu_asset_find (nu_asset_manager_t *manager, nu_uid_t uid)
{
    for (nu_size_t i = 0; i < manager->_entries.capacity; ++i)
    {
        // if (manager->_entries.data[i].value.)
    }
    return NU_NULL;
}
nu_bool_t
nu_asset_find_handle (const nu_asset_manager_t *manager,
                      nu_uid_t                  uid,
                      nu_asset_handle_t        *handle)
{
    return NU_FALSE;
}
nu_error_t
nu_asset_load_filename (nu_asset_manager_t *manager,
                        nu_asset_type_t     type,
                        const nu_char_t    *filename,
                        nu_asset_handle_t  *handle)
{
    void *data          = nu_asset_add(manager, type, NU_UID(filename), handle);
    nu__asset_type_t *t = manager->_types + type;

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
