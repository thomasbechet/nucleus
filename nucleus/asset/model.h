#ifndef NU_ASSET_MODEL_H
#define NU_ASSET_MODEL_H

#include <nucleus/asset/manager.h>
#include <nucleus/graphics.h>

NU_API nu_error_t nu_model_loader_init(nu_allocator_t    *alloc,
                                       nu_renderer_t     *renderer,
                                       nu_asset_loader_t *loader);
NU_API void       nu_model_loader_free(nu_asset_loader_t *loader,
                                       nu_allocator_t    *alloc);

#endif
