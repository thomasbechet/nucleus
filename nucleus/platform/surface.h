#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/platform/context.h>

NU_API nu_error_t nu_render(nu_platform_t *platform);
// NU_API const nu_texture_t *nu_surface_color_target(const nu_context_t *ctx);

NU_API nu_error_t nuext_set_viewport_mode(nu_platform_t        *platform,
                                          nuext_viewport_mode_t mode);

#endif
