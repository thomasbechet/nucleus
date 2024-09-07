#ifndef NU_ASSET_IMPL_H
#define NU_ASSET_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/graphics/graphics.h>
#include <nucleus/importer/importer.h>

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

static nu_asset_data_t
nu__asset_data_invalid (nu_asset_type_t type)
{
    nu_asset_data_t data;
    switch (type)
    {
        case NU_ASSET_TEXTURE:
            data.texture = NU_HANDLE_INVALID(nu_texture_t);
            break;
        case NU_ASSET_MATERIAL:
            data.material = NU_HANDLE_INVALID(nu_material_t);
            break;
        case NU_ASSET_MESH:
            data.mesh = NU_HANDLE_INVALID(nu_mesh_t);
            break;
        case NU_ASSET_MODEL:
            data.model = NU_HANDLE_INVALID(nu_model_t);
            break;
        case NU_ASSET_FONT:
            data.font = NU_HANDLE_INVALID(nu_font_t);
            break;
        case NU_ASSET_INPUT:
            data.input = NU_HANDLE_INVALID(nu_input_t);
            break;
        case NU_ASSET_CUSTOM:
            data.custom = NU_NULL;
            break;
    }
    return data;
}

nu_asset_t
nu_asset_add (nu_asset_type_t type, const nu_char_t *name)
{
    if (nu_asset_find(type, name).id != NU_HANDLE_INVALID_ID)
    {
        return NU_HANDLE_INVALID(nu_asset_t);
    }

    nu_asset_t         handle;
    nu__asset_entry_t *entry = nu_pool_add(&_ctx.asset.entries, &handle.id);
    entry->used              = NU_TRUE;
    entry->type              = type;
    entry->hash              = nu_hash(name);
    entry->bundle            = _ctx.asset.active_bundle;
    entry->refcount          = 0;
    entry->data              = nu__asset_data_invalid(type);

    return handle;
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
            return (nu_asset_t) { .id = i };
        }
    }
    return NU_HANDLE_INVALID(nu_asset_t);
}
nu_asset_data_t
nu_asset_data (nu_asset_t handle)
{
    nu_handle_assert(handle);
    return _ctx.asset.entries.data[handle.id].data;
}
nu_asset_info_t
nu_asset_info (nu_asset_t handle)
{
    nu_asset_info_t info;
    info.type = NU_ASSET_TEXTURE;
    return info;
}

nu_asset_t
nuext_asset_load_filename (nu_asset_type_t  type,
                           const nu_char_t *name,
                           const nu_char_t *filename)
{
    nu_asset_t handle = nu_asset_add(type, name);
    nu_handle_check(handle, return handle);
    nu_asset_data_t *data = &_ctx.asset.entries.data[handle.id].data;

    switch (type)
    {
        case NU_ASSET_TEXTURE: {
            nu_image_t image = nuext_image_load_filename(filename);
            nu_handle_check(image, return NU_HANDLE_INVALID(nu_asset_t));
            data->texture = nu_texture_create_image(image);
            nu_image_delete(image);
        }
        break;
        case NU_ASSET_MATERIAL:
        case NU_ASSET_MESH:
        case NU_ASSET_MODEL:
            data->model = nuext_model_load_filename(filename);
            break;
        case NU_ASSET_FONT:
        case NU_ASSET_INPUT:
        case NU_ASSET_CUSTOM:
            break;
    }

    return handle;
}

#endif
