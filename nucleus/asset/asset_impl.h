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

nu_asset_t
nu_asset_add (nu_asset_type_t type, nu_uid_t uid)
{
    nu_asset_t         handle;
    nu__asset_entry_t *entry = nu_pool_add(&_ctx.asset.entries, &handle._index);
    entry->used              = NU_TRUE;
    entry->type              = type;
    entry->uid               = uid;
    entry->bundle            = _ctx.asset.active_bundle;
    entry->refcount          = 0;

    return handle;
}
nu_asset_t
nu_asset_find (nu_asset_type_t type, nu_uid_t uid)
{
    for (nu_size_t i = 0; i < _ctx.asset.entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = _ctx.asset.entries.data + i;
        if (entry->used && entry->type == type && entry->uid == uid)
        {
            return (nu_asset_t) { ._index = i };
        }
    }
    return NU_INVALID_HANDLE(nu_asset_t);
}

nu_asset_data_t *
nu_asset_data (nu_asset_t handle)
{
    return &_ctx.asset.entries.data[handle._index].data;
}
nu_asset_type_t
nu_asset_type (nu_asset_t handle)
{
    return _ctx.asset.entries.data[handle._index].type;
}
nu_bundle_t
nu_asset_bundle (nu_asset_t handle)
{
    return _ctx.asset.entries.data[handle._index].bundle;
}

nu_asset_t
nuext_asset_load_filename (nu_asset_type_t  type,
                           nu_uid_t         uid,
                           const nu_char_t *filename)
{
    nu_asset_t handle = nu_asset_add(type, uid);

    return handle;
}

#endif
