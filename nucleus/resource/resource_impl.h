#ifndef NU_RESOURCE_IMPL_H
#define NU_RESOURCE_IMPL_H

#include <nucleus/internal.h>

static const nu_str_t nu__resource_type_names[]
    = { NU_STR("texture"), NU_STR("cubemap"), NU_STR("material"),
        NU_STR("model"),   NU_STR("input"),   NU_STR("unknown") };

static nu_error_t
nu__resource_init (void)
{
    NU_POOL_INIT(10, &_ctx.resource.entries);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__resource_free (void)
{
    NU_POOL_FREE(&_ctx.resource.entries);
    return NU_ERROR_NONE;
}

nu_resource_t
nu_resource_add (nu_resource_type_t type, nu_str_t name)
{
    if (nu_resource_exists(type, name))
    {
        NU_ERROR("resource '" NU_STR_FMT "' of type '" NU_STR_FMT
                 "' already exists",
                 NU_STR_ARGS(name),
                 NU_STR_ARGS(nu__resource_type_names[type]));
        return NU_NULL;
    }

    if (name.size > NU_RESOURCE_NAME_MAX)
    {
        NU_ERROR("asset name too long");
        return NU_NULL;
    }

    nu_size_t             index;
    nu__resource_entry_t *entry = NU_POOL_ADD(&_ctx.resource.entries, &index);
    entry->used                 = NU_TRUE;
    entry->type                 = type;
    entry->hash                 = nu_str_hash(name);
    entry->bundle               = _ctx.resource.active_bundle;
    entry->refcount             = 0;
    entry->data                 = NU_NULL;
    nu_memcpy(entry->name, name.data, name.size);
    entry->name_size = name.size;

    return NU_HANDLE_MAKE(nu_resource_t, index);
}
nu_resource_t
nu_resource_find (nu_resource_type_t type, nu_str_t name)
{
    nu_u32_t hash = nu_str_hash(name);
    for (nu_size_t i = 0; i < _ctx.resource.entries.capacity; ++i)
    {
        nu__resource_entry_t *entry = _ctx.resource.entries.data + i;
        if (entry->used && entry->type == type && entry->hash == hash)
        {
            return NU_HANDLE_MAKE(nu_resource_t, i);
        }
    }
    NU_ERROR("resource '" NU_STR_FMT "' of type '" NU_STR_FMT "' not found",
             NU_STR_ARGS(name),
             NU_STR_ARGS(nu__resource_type_names[type]));
    return NU_NULL;
}
nu_bool_t
nu_resource_exists (nu_resource_type_t type, nu_str_t name)
{
    nu_u32_t hash = nu_str_hash(name);
    for (nu_size_t i = 0; i < _ctx.resource.entries.capacity; ++i)
    {
        nu__resource_entry_t *entry = _ctx.resource.entries.data + i;
        if (entry->used && entry->type == type && entry->hash == hash)
        {
            return NU_TRUE;
        }
    }
    return NU_FALSE;
}
void *
nu_resource_data (nu_resource_t handle)
{
    NU_ASSERT(handle);
    return _ctx.resource.entries.data[NU_HANDLE_INDEX(handle)].data;
}
nu_resource_info_t
nu_resource_info (nu_resource_t handle)
{
    nu_resource_info_t info;
    info.type = NU_RESOURCE_TEXTURE;
    return info;
}

#endif
