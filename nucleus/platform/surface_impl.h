#ifndef NU_SURFACE_IMPL_H
#define NU_SURFACE_IMPL_H

#include <nucleus/internal.h>

static void
nu__update_viewport (nu__viewport_t *v)
{
    nu_v2_t global_pos  = nu_v2(v->extent.min.x, v->extent.min.y);
    nu_v2_t global_size = nu_v2_v2u(nu_b2i_size(v->extent));

    nu_f32_t aspect_ratio = (nu_f32_t)v->screen.x / (nu_f32_t)v->screen.y;

    nu_v2_t size = NU_V2_ZEROS;
    switch (v->mode)
    {
        case NUEXT_VIEWPORT_FIXED: {
            size = nu_v2((nu_f32_t)v->screen.x * v->scale_factor,
                         (nu_f32_t)v->screen.y * v->scale_factor);
        };
        break;
        case NUEXT_VIEWPORT_FIXED_BEST_FIT: {
            nu_f32_t w_factor = global_size.x / (nu_f32_t)v->screen.x;
            nu_f32_t h_factor = global_size.y / (nu_f32_t)v->screen.y;
            nu_f32_t min = NU_MAX(1.0f, nu_floor(NU_MIN(w_factor, h_factor)));
            size.x       = v->screen.x * min;
            size.y       = v->screen.y * min;
        }
        break;
        case NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT: {
            if (global_size.x / global_size.y >= aspect_ratio)
            {
                size.x = nu_floor(global_size.y * aspect_ratio);
                size.y = nu_floor(global_size.y);
            }
            else
            {
                size.x = nu_floor(global_size.x);
                size.y = nu_floor(global_size.x / aspect_ratio);
            }
        }
        break;
        case NUEXT_VIEWPORT_STRETCH:
            size = global_size;
            break;
    }

    nu_v2_t vpos = nu_v2_sub(global_size, size);
    vpos         = nu_v2_divs(vpos, 2);
    vpos         = nu_v2_add(vpos, global_pos);
    v->viewport  = nu_b2i_xywh(vpos.x, vpos.y, size.x, size.y);
}

static void
nu__window_size_callback (RGFW_window *window, RGFW_rect r)
{
    _ctx.platform.viewport.extent
        = nu_b2i_resize(_ctx.platform.viewport.extent, nu_v2u(r.w, r.h));
    nu__update_viewport(&_ctx.platform.viewport);
}

void
nu_config_surface_size (nu_u32_t width, nu_u32_t height)
{
    nu__config_t *cfg    = nu__config();
    cfg->platform.width  = width;
    cfg->platform.height = height;
}
void
nu_swap_buffers (void)
{
#ifdef NU_BUILD_GRAPHICS
    nu__graphics_render();
#endif
#ifdef NU_BUILD_UTILS
    nu__stats_update();
#endif
    RGFW_window_swapBuffers(_ctx.platform.win);
}
nu_f32_t
nu_surface_aspect (void)
{
    return (nu_f32_t)_ctx.platform.size.x / (nu_f32_t)_ctx.platform.size.y;
}

nu_error_t
nuext_viewport_mode (nuext_viewport_mode_t mode)
{
    return NU_ERROR_NONE;
}

#endif
