#ifndef NU_UTILS_STATS_IMPL_H
#define NU_UTILS_STATS_IMPL_H

#include <nucleus/internal.h>

static void
nu__stats_init (void)
{
    nu_timer_reset(&_ctx.utils.stats.timer);
    NU_ARRAY_FILL(_ctx.utils.stats.frames, NU__STATS_FRAME_COUNT, 0);
    _ctx.utils.stats.head  = 0;
    _ctx.utils.stats.avg   = 0;
    _ctx.utils.stats.frame = 0;

    _ctx.utils.stats.renderer_current.point_count      = 0;
    _ctx.utils.stats.renderer_current.line_count       = 0;
    _ctx.utils.stats.renderer_current.triangle_count   = 0;
    _ctx.utils.stats.renderer_current.renderpass_count = 0;
    _ctx.utils.stats.renderer_frame = _ctx.utils.stats.renderer_current;
}
static void
nu__stats_update (void)
{
    nu__stats_t *s = &_ctx.utils.stats;

    float delta = nu_timer_elapsed(&s->timer);
    nu_timer_reset(&s->timer);
    float frame_fps    = nu_floor((1.0 / delta) * 1000.0);
    s->frames[s->head] = frame_fps;
    s->head            = (s->head + 1) % NU__STATS_FRAME_COUNT;
    s->avg             = 0;
    for (nu_size_t i = 0; i < NU__STATS_FRAME_COUNT; ++i)
    {
        s->avg += s->frames[i];
    }
    s->avg /= NU__STATS_FRAME_COUNT;

    ++s->frame;

    _ctx.utils.stats.renderer_frame = _ctx.utils.stats.renderer_current;
    _ctx.utils.stats.renderer_current.point_count      = 0;
    _ctx.utils.stats.renderer_current.line_count       = 0;
    _ctx.utils.stats.renderer_current.triangle_count   = 0;
    _ctx.utils.stats.renderer_current.renderpass_count = 0;
}

void
nu_draw_stats (nu_renderpass_t pass, nu_font_t font, nu_vec2i_t pos)
{
    nu__stats_t *s = &_ctx.utils.stats;
    nu_char_t    string[256];
    nu_size_t    n;

    n = nu_snprintf(string, 256, "FPS:%d", (nu_u32_t)s->avg);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 0)));
    n = nu_snprintf(string, 256, "FRA:%d", s->frame);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 10)));
    n = nu_snprintf(
        string, 256, "RES:%dx%d", _ctx.platform.size.x, _ctx.platform.size.y);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 20)));
    n = nu_snprintf(string, 256, "TRI:%d", s->renderer_frame.triangle_count);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 30)));
    n = nu_snprintf(string, 256, "LIN:%d", s->renderer_frame.line_count);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 40)));
    n = nu_snprintf(string, 256, "PTS:%d", s->renderer_frame.point_count);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 50)));
    n = nu_snprintf(string, 256, "RDP:%d", s->renderer_frame.renderpass_count);
    nu_draw_text(pass, font, string, n, nu_vec2i_add(pos, nu_vec2i(0, 60)));
}

#endif
