#ifndef NU_SURFACE_IMPL_H
#define NU_SURFACE_IMPL_H

#include <nucleus/internal.h>

static void
nu__update_viewport (nu__viewport_t *v)
{
    nu_vec2_t global_pos  = nu_vec2(v->extent.p.x, v->extent.p.y);
    nu_vec2_t global_size = nu_vec2(v->extent.s.x, v->extent.s.y);

    float aspect_ratio = (float)v->screen.x / (float)v->screen.y;

    nu_vec2_t size = NU_VEC2_ZEROS;
    switch (v->mode)
    {
        case NUEXT_VIEWPORT_FIXED: {
            size = nu_vec2((float)v->screen.x * v->scale_factor,
                           (float)v->screen.y * v->scale_factor);
        };
        break;
        case NUEXT_VIEWPORT_FIXED_BEST_FIT: {
            float w_factor = global_size.x / (float)v->screen.x;
            float h_factor = global_size.y / (float)v->screen.y;
            float min      = NU_MAX(1.0f, nu_floor(NU_MIN(w_factor, h_factor)));
            size.x         = v->screen.x * min;
            size.y         = v->screen.y * min;
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

    nu_vec2_t vpos = nu_vec2_sub(global_size, size);
    vpos           = nu_vec2_divs(vpos, 2);
    vpos           = nu_vec2_add(vpos, global_pos);
    v->viewport    = nu_box2i(vpos.x, vpos.y, size.x, size.y);
}

static void
nu__window_size_callback (RGFW_window *window, RGFW_rect r)
{
    _ctx.platform.viewport.extent.s.x = r.w;
    _ctx.platform.viewport.extent.s.y = r.h;
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
float
nu_surface_aspect (void)
{
    return (float)_ctx.platform.size.x / (float)_ctx.platform.size.y;
}

nu_error_t
nuext_viewport_mode (nuext_viewport_mode_t mode)
{
    return NU_ERROR_NONE;
}

#endif
