#ifndef NU_ASSET_IMPL_H
#define NU_ASSET_IMPL_H

#include <nucleus/internal.h>

static const nu_str_t nu__asset_type_names[]
    = { NU_STR("texture"), NU_STR("cubemap"), NU_STR("material"),
        NU_STR("model"),   NU_STR("input"),   NU_STR("table"),
        NU_STR("unknown") };

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
nu_asset_add (nu_asset_type_t type, nu_str_t name)
{
    if (nu_asset_exists(type, name))
    {
        NU_ERROR("asset '" NU_STR_FORMAT "' of type '" NU_STR_FORMAT
                 "' already exists",
                 NU_STR_ARGS(name),
                 NU_STR_ARGS(nu__asset_type_names[type]));
        return NU_NULL;
    }

    if (name.size > NU_ASSET_NAME_MAX)
    {
        NU_ERROR("asset name too long");
        return NU_NULL;
    }

    nu_size_t          index;
    nu__asset_entry_t *entry = NU_POOL_ADD(&_ctx.asset.entries, &index);
    entry->used              = NU_TRUE;
    entry->type              = type;
    entry->hash              = nu_str_hash(name);
    entry->bundle            = _ctx.asset.active_bundle;
    entry->refcount          = 0;
    entry->data              = NU_NULL;
    nu_memcpy(entry->name, name.data, name.size);
    entry->name_size = name.size;

    return NU_HANDLE_MAKE(nu_asset_t, index);
}
nu_asset_t
nu_asset_find (nu_asset_type_t type, nu_str_t name)
{
    nu_u32_t hash = nu_str_hash(name);
    for (nu_size_t i = 0; i < _ctx.asset.entries.capacity; ++i)
    {
        nu__asset_entry_t *entry = _ctx.asset.entries.data + i;
        if (entry->used && entry->type == type && entry->hash == hash)
        {
            return NU_HANDLE_MAKE(nu_asset_t, i);
        }
    }
    NU_ERROR("asset '" NU_STR_FORMAT "' of type '" NU_STR_FORMAT "' not found",
             NU_STR_ARGS(name),
             NU_STR_ARGS(nu__asset_type_names[type]));
    return NU_NULL;
}
nu_bool_t
nu_asset_exists (nu_asset_type_t type, nu_str_t name)
{
    nu_u32_t hash = nu_str_hash(name);
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
    NU_ASSERT(handle);
    return _ctx.asset.entries.data[NU_HANDLE_INDEX(handle)].data;
}
nu_asset_info_t
nu_asset_info (nu_asset_t handle)
{
    nu_asset_info_t info;
    info.type = NU_ASSET_TEXTURE;
    return info;
}

#endif
