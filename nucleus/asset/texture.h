#ifndef NU_ASSET_TEXTURE_H
#define NU_ASSET_TEXTURE_H

#include <nucleus/asset/manager.h>

typedef struct
{
    nu_texture_handle_t handle;
    nu_texture_info_t   info;
    nu_bool_t           has_image;
} nu_texture_t;

typedef struct
{
    nu_renderer_t  *_renderer;
    nu_allocator_t *_allocator;
} nu_texture_loader_t;

NU_API nu_error_t nu_texture_loader_init(nu_allocator_t    *alloc,
                                         nu_renderer_t     *renderer,
                                         nu_asset_loader_t *loader);
NU_API void       nu_texture_loader_free(nu_asset_loader_t *loader,
                                         nu_allocator_t    *alloc);

#endif
