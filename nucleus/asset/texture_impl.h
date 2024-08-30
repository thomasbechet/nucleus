#ifndef NU_ASSET_TEXTURE_IMPL_H
#define NU_ASSET_TEXTURE_IMPL_H

#include <nucleus/asset/texture.h>
#include <nucleus/utils.h>

static nu_error_t
nu__texture_loader_load_async (void                       *loader,
                               const nu_asset_load_info_t *info,
                               void                       *data)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__texture_loader_load_sync (void                       *loader,
                              const nu_asset_load_info_t *info,
                              void                       *data)
{
    NU_ASSERT(info->filename);

    nu_texture_loader_t *context = loader;
    nu_texture_t        *texture = data;

    nu_image_t image;
    nu_error_t error = nuext_load_image_filename(
        info->filename, context->_allocator, &image);
    NU_ERROR_CHECK(error, return error);

    texture->info.size   = image.size;
    texture->info.usage  = NU_TEXTURE_USAGE_SAMPLE;
    texture->info.format = NU_TEXTURE_FORMAT_COLOR;
    texture->info.colors = image.data;
    texture->has_image   = NU_FALSE;
    error                = nu_texture_create(
        context->_renderer, &texture->info, &texture->handle);
    nu_image_free(&image, context->_allocator);
    NU_ERROR_CHECK(error, return error);

    return NU_ERROR_NONE;
}
static nu_error_t
nu__texture_loader_unload (void *loader, void *data)
{
    return NU_ERROR_NONE;
}

nu_error_t
nu_texture_loader_init (nu_allocator_t    *alloc,
                        nu_renderer_t      renderer,
                        nu_asset_loader_t *loader)
{
    nu_texture_loader_t *data = nu_alloc(alloc, sizeof(*data));
    data->_allocator          = alloc;
    data->_renderer           = renderer;

    loader->loader     = data;
    loader->load_async = nu__texture_loader_load_async;
    loader->load_sync  = nu__texture_loader_load_sync;
    loader->unload     = nu__texture_loader_unload;

    return NU_ERROR_NONE;
}
void
nu_texture_loader_free (nu_asset_loader_t *loader, nu_allocator_t *alloc)
{
    nu_free(alloc, loader, sizeof(nu_texture_loader_t));
}

#endif
