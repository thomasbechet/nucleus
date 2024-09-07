#ifndef NU_ASSET_IMPL_H
#define NU_ASSET_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/graphics.h>

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
    nu__asset_entry_t *entry = nu_pool_add(&_ctx.asset.entries, &handle.id);
    entry->used              = NU_TRUE;
    entry->type              = type;
    entry->uid               = uid;
    entry->bundle            = _ctx.asset.active_bundle;
    entry->refcount          = 0;
    switch (type)
    {
        case NU_ASSET_TEXTURE:
            entry->data.texture = NU_HANDLE_ERROR(nu_texture_t);
            break;
        case NU_ASSET_MATERIAL:
            entry->data.material = NU_HANDLE_ERROR(nu_material_t);
            break;
        case NU_ASSET_MESH:
            entry->data.mesh = NU_HANDLE_ERROR(nu_mesh_t);
            break;
        case NU_ASSET_MODEL:
            entry->data.model = NU_HANDLE_ERROR(nu_model_t);
            break;
        case NU_ASSET_FONT:
            entry->data.font = NU_HANDLE_ERROR(nu_font_t);
            break;
        case NU_ASSET_INPUT:
            break;
            entry->data.input = NU_HANDLE_ERROR(nu_input_t);
            break;
        case NU_ASSET_CUSTOM:
            entry->data.custom = NU_NULL;
            break;
    }

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
            return (nu_asset_t) { .id = i };
        }
    }
    return NU_HANDLE_ERROR(nu_asset_t);
}

nu_asset_data_t
nu_asset_data (nu_asset_t handle)
{
    NU_HANDLE_ASSERT(handle);
    return _ctx.asset.entries.data[handle.id].data;
}
nu_asset_type_t
nu_asset_type (nu_asset_t handle)
{
    NU_HANDLE_ASSERT(handle);
    return _ctx.asset.entries.data[handle.id].type;
}
nu_bundle_t
nu_asset_bundle (nu_asset_t handle)
{
    NU_HANDLE_ASSERT(handle);
    return _ctx.asset.entries.data[handle.id].bundle;
}

nu_asset_t
nuext_asset_load_filename (nu_asset_type_t  type,
                           nu_uid_t         uid,
                           const nu_char_t *filename)
{
    nu_asset_t       handle = nu_asset_add(type, uid);
    nu_asset_data_t *data   = &_ctx.asset.entries.data[handle.id].data;

    switch (type)
    {
        case NU_ASSET_TEXTURE:
            // data->texture = nuext_texture_load_filename(filename);
            break;
        case NU_ASSET_MATERIAL:
        case NU_ASSET_MESH:
        case NU_ASSET_MODEL:
        case NU_ASSET_FONT:
        case NU_ASSET_INPUT:
        case NU_ASSET_CUSTOM:
            break;
    }

    return handle;
}

#endif
