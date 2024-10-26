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

    _ctx.utils.stats.graphics_current.point_count      = 0;
    _ctx.utils.stats.graphics_current.line_count       = 0;
    _ctx.utils.stats.graphics_current.triangle_count   = 0;
    _ctx.utils.stats.graphics_current.renderpass_count = 0;
    _ctx.utils.stats.graphics_frame = _ctx.utils.stats.graphics_current;
}
static void
nu__stats_update (void)
{
    nu__stats_t *s = &_ctx.utils.stats;

    nu_f32_t delta = nu_timer_elapsed(&s->timer);
    nu_timer_reset(&s->timer);
    nu_f32_t frame_fps = nu_floor((1.0 / delta) * 1000.0);
    s->frames[s->head] = frame_fps;
    s->head            = (s->head + 1) % NU__STATS_FRAME_COUNT;
    s->avg             = 0;
    for (nu_size_t i = 0; i < NU__STATS_FRAME_COUNT; ++i)
    {
        s->avg += s->frames[i];
    }
    s->avg /= NU__STATS_FRAME_COUNT;

    ++s->frame;

    _ctx.utils.stats.graphics_frame = _ctx.utils.stats.graphics_current;
    _ctx.utils.stats.graphics_current.point_count      = 0;
    _ctx.utils.stats.graphics_current.line_count       = 0;
    _ctx.utils.stats.graphics_current.triangle_count   = 0;
    _ctx.utils.stats.graphics_current.renderpass_count = 0;
}

void
nu_draw_stats (nu_renderpass_t pass, nu_font_t font, nu_v2i_t pos)
{
    nu__stats_t    *s = &_ctx.utils.stats;
    nu_str_t        str;
    nu_byte_t       str_buf[256];
    const nu_size_t buf_size = 256;

    str = nu_snprintf(str_buf, buf_size, NU_STR("FPS:%d"), (nu_u32_t)s->avg);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 0)));
    str = nu_snprintf(str_buf, buf_size, NU_STR("FRA:%d"), s->frame);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 10)));
    str = nu_snprintf(str_buf,
                      buf_size,
                      NU_STR("RES:%dx%d"),
                      _ctx.platform.size.x,
                      _ctx.platform.size.y);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 20)));
    str = nu_snprintf(
        str_buf, buf_size, NU_STR("TRI:%d"), s->graphics_frame.triangle_count);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 30)));
    str = nu_snprintf(
        str_buf, buf_size, NU_STR("LIN:%d"), s->graphics_frame.line_count);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 40)));
    str = nu_snprintf(
        str_buf, buf_size, NU_STR("PTS:%d"), s->graphics_frame.point_count);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 50)));
    str = nu_snprintf(str_buf,
                      buf_size,
                      NU_STR("RDP:%d"),
                      s->graphics_frame.renderpass_count);
    nu_draw_text(pass, str, font, nu_v2i_add(pos, nu_v2i(0, 60)));
}

#endif
