#ifndef NU_ASSET_IMPL_H
#define NU_ASSET_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/asset/texture_impl.h>
#include <nucleus/asset/model_impl.h>

static nu_error_t
nu__asset_init (void)
{
    nu_pool_init(10, &_ctx.asset.entries);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__asset_free (void)
{
    // Free base loaders
    nu_pool_free(&_ctx.asset.entries);
    return NU_ERROR_NONE;
}

void *
nu_asset_add (nu_asset_type_t type, nu_uid_t uid, nu_asset_handle_t *handle)
{
    nu_size_t          index;
    nu__asset_entry_t *entry = nu_pool_add(&_ctx.asset.entries, &index);
    entry->type              = type;
    entry->uid               = uid;
    // entry->data = nu_alloc(manager._ptr->allocator, t->size);
    entry->used = NU_TRUE;

    if (handle)
    {
        handle->_index = index;
    }

    return entry->data;
}
void *
nu_asset_get (nu_asset_handle_t handle)
{
    return _ctx.asset.entries.data + handle._index;
}
void *
nu_asset_find (nu_asset_type_t type, nu_uid_t uid, nu_asset_handle_t *handle)
{
    nu_asset_handle_t h;
    if (nu_asset_find_handle(type, uid, &h))
    {
        if (handle)
        {
            *handle = h;
        }
        return _ctx.asset.entries.data[h._index].data;
    }
    return NU_NULL;
}
nu_bool_t
nu_asset_find_handle (nu_asset_type_t    type,
                      nu_uid_t           uid,
                      nu_asset_handle_t *handle)
{
    NU_ASSERT(handle);
    for (nu_size_t i = 0; i < _ctx.asset.entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = _ctx.asset.entries.data + i;
        if (entry->used && entry->type == type && entry->uid == uid)
        {
            handle->_index = i;
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
nu_error_t
nuext_asset_load_filename (nu_asset_type_t    type,
                           const nu_char_t   *filename,
                           const nu_char_t   *name,
                           nu_asset_handle_t *handle)
{
    void *data = nu_asset_add(type, NU_UID(name), handle);

    nu_error_t error = NU_ERROR_NONE;

    return error;
}

#endif
