#ifndef NU_ASSET_TEXTURE_IMPL_H
#define NU_ASSET_TEXTURE_IMPL_H

#include <nucleus/asset/texture.h>

static nu_error_t
nu__texture_loader_load_sync (void                       *loader,
                              const nu_asset_load_info_t *info,
                              void                       *data)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__texture_loader_load_async (void                       *loader,
                               const nu_asset_load_info_t *info,
                               void                       *data)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__texture_loader_unload (void *loader, void *data)
{
    return NU_ERROR_NONE;
}

nu_error_t
nu_texture_loader_init (nu_allocator_t    *alloc,
                        nu_renderer_t     *renderer,
                        nu_asset_loader_t *loader)
{
    loader->loader     = renderer;
    loader->load_sync  = nu__texture_loader_load_sync;
    loader->load_async = nu__texture_loader_load_async;
    loader->unload     = nu__texture_loader_unload;
    return NU_ERROR_NONE;
}
void
nu_texture_loader_free (nu_asset_loader_t *loader, nu_allocator_t *alloc)
{
}

#endif
