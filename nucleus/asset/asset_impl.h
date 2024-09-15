#ifndef NU_ASSET_IMPL_H
#define NU_ASSET_IMPL_H

#include <nucleus/internal.h>

static const nu_char_t *nu__asset_type_names[] = {
    "texture", "cubemap", "material", "model", "input", "table", "unknown"
};

static nu_error_t
nu__asset_init (void)
{
    NU_POOL_INIT(10, &_ctx.asset.entries);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__asset_free (void)
{
    // Free base loaders
    NU_POOL_FREE(&_ctx.asset.entries);
    return NU_ERROR_NONE;
}

nu_asset_t
nu_asset_add (nu_asset_type_t type, const nu_char_t *name)
{
    if (nu_asset_exists(type, name))
    {
        NU_ERROR("asset '%s' of type '%s' already exists",
                 name,
                 nu__asset_type_names[type]);
        return NU_NULL;
    }

    nu_size_t          index;
    nu__asset_entry_t *entry = NU_POOL_ADD(&_ctx.asset.entries, &index);
    entry->used              = NU_TRUE;
    entry->type              = type;
    entry->hash              = nu_hash(name);
    entry->bundle            = _ctx.asset.active_bundle;
    entry->refcount          = 0;
    entry->data              = NU_NULL;
    nu_strncpy(entry->name, name, NU_ASSET_NAME_MAX);

    return nu_handle_make(nu_asset_t, index);
}
nu_asset_t
nu_asset_find (nu_asset_type_t type, const nu_char_t *name)
{
    nu_u32_t hash = nu_hash(name);
    for (nu_size_t i = 0; i < _ctx.asset.entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = _ctx.asset.entries.data + i;
        if (entry->used && entry->type == type && entry->hash == hash)
        {
            return nu_handle_make(nu_asset_t, i);
        }
    }
    NU_ERROR(
        "asset '%s' of type '%s' not found", name, nu__asset_type_names[type]);
    return NU_NULL;
}
nu_bool_t
nu_asset_exists (nu_asset_type_t type, const nu_char_t *name)
{
    nu_u32_t hash = nu_hash(name);
    for (nu_size_t i = 0; i < _ctx.asset.entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = _ctx.asset.entries.data + i;
        if (entry->used && entry->type == type && entry->hash == hash)
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
void *
nu_asset_data (nu_asset_t handle)
{
    nu_assert(handle);
    return _ctx.asset.entries.data[nu_handle_index(handle)].data;
}
nu_asset_info_t
nu_asset_info (nu_asset_t handle)
{
    nu_asset_info_t info;
    info.type = NU_ASSET_TEXTURE;
    return info;
}

#endif
