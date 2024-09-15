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
    _ctx.platform.size
        = nu_uvec2(_ctx.config.platform.width, _ctx.config.platform.height);
    _ctx.platform.close_requested = NU_FALSE;

    NU_INFO("initialize platform context (%dx%d)",
            _ctx.config.platform.width,
            _ctx.config.platform.height);

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
    _ctx.platform.viewport.extent   = nu_rect(0, 0, width, height);
    _ctx.platform.viewport.viewport = nu_rect(0, 0, width, height);
    nu__update_viewport(&_ctx.platform.viewport);

    // Bind callbacks
    RGFW_window_makeCurrent_OpenGL(_ctx.platform.win);

    RGFW_setKeyCallback(nu__key_callback);
    RGFW_setMouseButtonCallback(nu__mouse_button_callback);
    RGFW_setMousePosCallback(nu__mouse_position_callback);
    RGFW_setWindowResizeCallback(nu__window_size_callback);

    // Get initial mouse position
    RGFW_point p                 = RGFW_window_getMousePoint(_ctx.platform.win);
    _ctx.platform.mouse_position = nu_vec2((float)p.x, (float)p.y);
    _ctx.platform.mouse_old_position = _ctx.platform.mouse_position;

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
    _ctx.platform.mouse_x_first_binding            = NU__ID_NONE;
    _ctx.platform.mouse_y_first_binding            = NU__ID_NONE;
    _ctx.platform.mouse_motion_x_pos_first_binding = NU__ID_NONE;
    _ctx.platform.mouse_motion_x_neg_first_binding = NU__ID_NONE;
    _ctx.platform.mouse_motion_y_pos_first_binding = NU__ID_NONE;
    _ctx.platform.mouse_motion_y_neg_first_binding = NU__ID_NONE;

    return NU_ERROR_NONE;
}
static nu_error_t
nu__platform_free (void)
{
    nu_error_t error;

    NU_INFO("terminate platform context");

    RGFW_window_close(_ctx.platform.win);

    NU_POOL_FREE(&_ctx.platform.bindings);
    NU_POOL_FREE(&_ctx.platform.entries);

    return NU_ERROR_NONE;
}
void
nu_poll_events (void)
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
        _ctx.platform.mouse_scroll = NU_VEC2_ZERO;

        // Poll events
        RGFW_window_checkEvents(_ctx.platform.win, 0);

        // Check close requested
        _ctx.platform.close_requested
            = RGFW_window_shouldClose(_ctx.platform.win);

        // Check capture mode
        if (_ctx.platform.switch_capture_mouse)
        {
            _ctx.platform.capture_mouse = !_ctx.platform.capture_mouse;
            if (_ctx.platform.capture_mouse)
            {
                RGFW_window_mouseHold(_ctx.platform.win,
                                      RGFW_AREA(_ctx.platform.size.x / 2,
                                                _ctx.platform.size.y / 2));
                RGFW_window_showMouse(_ctx.platform.win, RGFW_FALSE);
            }
            else
            {
                RGFW_window_mouseUnhold(_ctx.platform.win);
                RGFW_window_showMouse(_ctx.platform.win, RGFW_TRUE);
            }
            _ctx.platform.switch_capture_mouse = NU_FALSE;
        }

        // Check fullscreen button
        if (_ctx.platform.switch_fullscreen)
        {
            if (_ctx.platform.fullscreen)
            {
                // const GLFWvidmode *mode
                //     = glfwGetVideoMode(glfwGetPrimaryMonitor());
                // glfwSetWindowMonitor(
                //     _ctx.platform.surface.glfw.win,
                //     NU_NULL,
                //     _ctx.platform.surface.glfw.previous_position.x,
                //     _ctx.platform.surface.glfw.previous_position.y,
                //     _ctx.platform.surface.glfw.previous_size.x,
                //     _ctx.platform.surface.glfw.previous_size.y,
                //     GLFW_DONT_CARE);
            }
            else
            {
                // int x, y;
                // glfwGetWindowSize(_ctx.platform.surface.glfw.win, &x, &y);
                // _ctx.platform.surface.glfw.previous_size = nu_uvec2(x, y);
                // glfwGetWindowPos(_ctx.platform.surface.glfw.win, &x, &y);
                // _ctx.platform.surface.glfw.previous_position = nu_uvec2(x,
                // y);
                //
                // const GLFWvidmode *mode
                //     = glfwGetVideoMode(glfwGetPrimaryMonitor());
                // glfwSetWindowMonitor(_ctx.platform.surface.glfw.win,
                //                      glfwGetPrimaryMonitor(),
                //                      0,
                //                      0,
                //                      mode->width,
                //                      mode->height,
                //                      GLFW_DONT_CARE);
            }
            _ctx.platform.switch_fullscreen = NU_FALSE;
            _ctx.platform.fullscreen        = !_ctx.platform.fullscreen;
        }

        // Update mouse motion
        if (NU_TRUE || _ctx.platform.capture_mouse)
        {
            nu_vec2_t mouse_motion = nu_vec2_sub(
                _ctx.platform.mouse_position, _ctx.platform.mouse_old_position);
            mouse_motion = nu_vec2_divs(mouse_motion, 200);
            if (mouse_motion.x != _ctx.platform.mouse_motion.x)
            {
                float pos_x = 0;
                float neg_x = 0;
                if (mouse_motion.x > 0)
                {
                    pos_x = mouse_motion.x;
                }
                else if (mouse_motion.x < 0)
                {
                    neg_x = nu_fabs(mouse_motion.x);
                }
                nu__dispatch_binding_axis(
                    _ctx.platform.mouse_motion_x_pos_first_binding, pos_x);
                nu__dispatch_binding_axis(
                    _ctx.platform.mouse_motion_x_neg_first_binding, neg_x);
            }
            if (mouse_motion.y != _ctx.platform.mouse_motion.y)
            {
                float pos_y = 0;
                float neg_y = 0;
                if (mouse_motion.y > 0)
                {
                    pos_y = mouse_motion.y;
                }
                else if (mouse_motion.y < 0)
                {
                    neg_y = nu_fabs(mouse_motion.y);
                }
                nu__dispatch_binding_axis(
                    _ctx.platform.mouse_motion_y_pos_first_binding, pos_y);
                nu__dispatch_binding_axis(
                    _ctx.platform.mouse_motion_y_neg_first_binding, neg_y);
            }
            _ctx.platform.mouse_motion = mouse_motion;

            // Update mouse position
            if (_ctx.platform.mouse_position.x
                    != _ctx.platform.mouse_old_position.x
                || _ctx.platform.mouse_position.y
                       != _ctx.platform.mouse_old_position.y)
            {
                if (nu_rect_contains(_ctx.platform.viewport.viewport,
                                     _ctx.platform.mouse_position))
                {
                    nu_vec2_t relpos
                        = nu_rect_normalize(_ctx.platform.viewport.viewport,
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

#endif
