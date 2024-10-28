#ifndef NU_BACKEND_IMPL_H
#define NU_BACKEND_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_GRAPHICS_GL
#include <nucleus/graphics/gl/gl_impl.h>
#endif

static void
nu__renderer_init (void)
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__init();
#endif
}
static void
nu__renderer_free (void)
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__free();
#endif
}
static void
nu__renderer_render (nu_b2i_t global_viewport, nu_b2i_t viewport)
{
#ifdef NU_BUILD_GRAPHICS_GL
    nugl__render(global_viewport, viewport);
#endif
}

nu_texture_t
nu_surface_color_target (void)
{
    return _ctx.graphics.surface_color;
}

#endif
