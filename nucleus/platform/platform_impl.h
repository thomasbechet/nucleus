#ifndef NU_PLATFORM_IMPL_H
#define NU_PLATFORM_IMPL_H

#include <nucleus/platform/input_impl.h>
#include <nucleus/platform/surface_impl.h>

#define GLAD_GL_IMPLEMENTATION
#include <nucleus/external/glad/gl.h>

static nu_error_t
nu__platform_init (void)
{
    nu_error_t error;

    // Initialize context
    NU_POOL_INIT(10, &_ctx.platform.entries);
    _ctx.platform.close_requested = NU_FALSE;

    // Initialize surface (and inputs)
    const nu_int_t width  = NU__DEFAULT_WINDOW_WIDTH;
    const nu_int_t height = NU__DEFAULT_WINDOW_HEIGHT;

    // Initialize values
    _ctx.platform.fullscreen           = NU_FALSE;
    _ctx.platform.switch_fullscreen    = NU_FALSE;
    _ctx.platform.capture_mouse        = NU_FALSE;
    _ctx.platform.switch_capture_mouse = NU_FALSE;
    nu_timer_reset(&_ctx.platform.last_mouse_click);

    // Create window
    _ctx.platform.win = RGFW_createWindow(
        "nucleus", RGFW_RECT(0, 0, width, height), RGFW_CENTER);

    // Initialize viewport
    _ctx.platform.viewport.mode     = NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT;
    _ctx.platform.viewport.screen   = _ctx.platform.size;
    _ctx.platform.viewport.extent   = nu_b2i_xywh(0, 0, width, height);
    _ctx.platform.viewport.viewport = nu_b2i_xywh(0, 0, width, height);
    nu__update_viewport(&_ctx.platform.viewport);

    // Bind callbacks
    RGFW_window_makeCurrent_OpenGL(_ctx.platform.win);
    RGFW_window_makeCurrent(_ctx.platform.win);

    // Get initial mouse position
    RGFW_point p                 = RGFW_window_getMousePoint(_ctx.platform.win);
    _ctx.platform.mouse_position = nu_v2((nu_f32_t)p.x, (nu_f32_t)p.y);
    _ctx.platform.mouse_old_position    = _ctx.platform.mouse_position;
    _ctx.platform.mouse_motion          = NU_V2_ZEROS;
    _ctx.platform.mouse_motion_previous = NU_V2_ZEROS;

    // Initialize inputs
    NU_POOL_INIT(10, &_ctx.platform.bindings);
    for (nu_u32_t i = 0; i < NU__MAX_KEY_COUNT; ++i)
    {
        _ctx.platform.key_to_first_binding[i] = NU__ID_NONE;
    }
    for (nu_u32_t i = 0; i < NU__MAX_MOUSE_BUTTON_COUNT; ++i)
    {
        _ctx.platform.mouse_button_to_first_binding[i] = NU__ID_NONE;
    }
    _ctx.platform.mouse_x_first_binding        = NU__ID_NONE;
    _ctx.platform.mouse_y_first_binding        = NU__ID_NONE;
    _ctx.platform.mouse_motion_x_first_binding = NU__ID_NONE;
    _ctx.platform.mouse_motion_y_first_binding = NU__ID_NONE;

    return NU_ERROR_NONE;
}
static nu_error_t
nu__platform_free (void)
{
    RGFW_window_close(_ctx.platform.win);

    NU_POOL_FREE(&_ctx.platform.bindings);
    NU_POOL_FREE(&_ctx.platform.entries);

    return NU_ERROR_NONE;
}
static void
nu__platform_poll_events (void)
{
    if (_ctx.platform.win)
    {
        // Update input states
        for (nu_size_t i = 0; i < _ctx.platform.entries.capacity; ++i)
        {
            if (_ctx.platform.entries.data[i].used)
            {
                _ctx.platform.entries.data[i].state.previous
                    = _ctx.platform.entries.data[i].state.value;
            }
        }

        // Reset mouse scroll
        _ctx.platform.mouse_scroll = NU_V2_ZEROS;
        _ctx.platform.mouse_motion = NU_V2_ZEROS;

        // Check close requested
        _ctx.platform.close_requested
            = RGFW_window_shouldClose(_ctx.platform.win);

        // Process events
        while (RGFW_window_checkEvent(_ctx.platform.win))
        {
            switch (_ctx.platform.win->event.type)
            {
                case RGFW_mousePosChanged:
                    nu__mouse_position_callback(_ctx.platform.win,
                                                _ctx.platform.win->event.point);
                    break;
                case RGFW_keyPressed:
                    nu__key_callback(_ctx.platform.win,
                                     _ctx.platform.win->event.keyCode,
                                     _ctx.platform.win->event.keyName,
                                     NU_FALSE,
                                     NU_TRUE);
                    break;
                case RGFW_keyReleased:
                    nu__key_callback(_ctx.platform.win,
                                     _ctx.platform.win->event.keyCode,
                                     _ctx.platform.win->event.keyName,
                                     NU_FALSE,
                                     NU_FALSE);
                    break;
                case RGFW_mouseButtonPressed:
                    nu__mouse_button_callback(_ctx.platform.win,
                                              _ctx.platform.win->event.button,
                                              _ctx.platform.win->event.scroll,
                                              NU_TRUE);
                    break;
                case RGFW_mouseButtonReleased:
                    nu__mouse_button_callback(_ctx.platform.win,
                                              _ctx.platform.win->event.button,
                                              _ctx.platform.win->event.scroll,
                                              NU_FALSE);
                    break;
                case RGFW_windowResized:
                    nu__window_size_callback(_ctx.platform.win,
                                             _ctx.platform.win->r);
                    break;
            }
        }

        // Check capture mode
        if (_ctx.platform.switch_capture_mouse)
        {
            _ctx.platform.capture_mouse = !_ctx.platform.capture_mouse;
            if (_ctx.platform.capture_mouse)
            {
                RGFW_window_showMouse(_ctx.platform.win, RGFW_FALSE);
                RGFW_window_mouseHold(_ctx.platform.win,
                                      RGFW_AREA(_ctx.platform.win->r.w / 2,
                                                _ctx.platform.win->r.h / 2));
            }
            else
            {
                RGFW_window_showMouse(_ctx.platform.win, RGFW_TRUE);
                RGFW_window_mouseUnhold(_ctx.platform.win);
            }
            _ctx.platform.switch_capture_mouse = NU_FALSE;
        }

        // Check fullscreen button
        if (_ctx.platform.switch_fullscreen)
        {
            if (_ctx.platform.fullscreen)
            {
                RGFW_window_resize(_ctx.platform.win,
                                   RGFW_AREA(_ctx.platform.previous_rect.w,
                                             _ctx.platform.previous_rect.h));
                RGFW_window_move(_ctx.platform.win,
                                 RGFW_POINT(_ctx.platform.previous_rect.x,
                                            _ctx.platform.previous_rect.y));
                RGFW_window_setBorder(_ctx.platform.win, RGFW_TRUE);
                nu__window_size_callback(_ctx.platform.win,
                                         _ctx.platform.win->r);
            }
            else
            {
                _ctx.platform.previous_rect = _ctx.platform.win->r;
                RGFW_monitor mon = RGFW_window_getMonitor(_ctx.platform.win);
                RGFW_window_moveToMonitor(_ctx.platform.win, mon);
                RGFW_window_setBorder(_ctx.platform.win, RGFW_FALSE);
                RGFW_window_resize(_ctx.platform.win,
                                   RGFW_AREA(mon.rect.w, mon.rect.h));
                RGFW_window_move(_ctx.platform.win,
                                 RGFW_POINT(mon.rect.x, mon.rect.y));
            }
            _ctx.platform.switch_fullscreen = NU_FALSE;
            _ctx.platform.fullscreen        = !_ctx.platform.fullscreen;
        }

        // Update mouse motion
        if (_ctx.platform.capture_mouse)
        {
            if (_ctx.platform.mouse_motion.x
                    != _ctx.platform.mouse_motion_previous.x
                || _ctx.platform.mouse_motion.y
                       != _ctx.platform.mouse_motion_previous.y)
            {
                nu__dispatch_binding_axis(
                    _ctx.platform.mouse_motion_x_first_binding,
                    _ctx.platform.mouse_motion.x);
                nu__dispatch_binding_axis(
                    _ctx.platform.mouse_motion_y_first_binding,
                    _ctx.platform.mouse_motion.y);

                _ctx.platform.mouse_motion_previous
                    = _ctx.platform.mouse_motion;
            }
        }
        else
        {
            if (_ctx.platform.mouse_position.x
                    != _ctx.platform.mouse_old_position.x
                || _ctx.platform.mouse_position.y
                       != _ctx.platform.mouse_old_position.y)
            {
                if (nu_b2i_contains(_ctx.platform.viewport.viewport,
                                    _ctx.platform.mouse_position))
                {
                    nu_v2_t relpos
                        = nu_b2i_normalize(_ctx.platform.viewport.viewport,
                                           _ctx.platform.mouse_position);
                    nu__dispatch_binding_axis(
                        _ctx.platform.mouse_x_first_binding, relpos.x);
                    nu__dispatch_binding_axis(
                        _ctx.platform.mouse_y_first_binding, relpos.y);
                }
            }
            _ctx.platform.mouse_old_position = _ctx.platform.mouse_position;
        }
    }
}
nu_bool_t
nu_exit_requested (void)
{
    return _ctx.platform.close_requested;
}
void
nu_request_stop (void)
{
    _ctx.platform.close_requested = NU_TRUE;
}

#endif
