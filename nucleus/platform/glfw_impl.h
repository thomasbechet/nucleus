#ifndef NUGLFW_IMPL_H
#define NUGLFW_IMPL_H

#include <nucleus/internal.h>

static int nuglfw__button_to_key[] = { GLFW_KEY_A,
                                       GLFW_KEY_B,
                                       GLFW_KEY_C,
                                       GLFW_KEY_D,
                                       GLFW_KEY_E,
                                       GLFW_KEY_F,
                                       GLFW_KEY_G,
                                       GLFW_KEY_H,
                                       GLFW_KEY_I,
                                       GLFW_KEY_J,
                                       GLFW_KEY_K,
                                       GLFW_KEY_L,
                                       GLFW_KEY_M,
                                       GLFW_KEY_N,
                                       GLFW_KEY_O,
                                       GLFW_KEY_P,
                                       GLFW_KEY_Q,
                                       GLFW_KEY_R,
                                       GLFW_KEY_S,
                                       GLFW_KEY_T,
                                       GLFW_KEY_U,
                                       GLFW_KEY_V,
                                       GLFW_KEY_W,
                                       GLFW_KEY_X,
                                       GLFW_KEY_Y,
                                       GLFW_KEY_Z,

                                       GLFW_KEY_F1,
                                       GLFW_KEY_F2,
                                       GLFW_KEY_F3,
                                       GLFW_KEY_F4,
                                       GLFW_KEY_F5,
                                       GLFW_KEY_F6,
                                       GLFW_KEY_F7,
                                       GLFW_KEY_F8,
                                       GLFW_KEY_F9,
                                       GLFW_KEY_F10,
                                       GLFW_KEY_F11,
                                       GLFW_KEY_F12,

                                       GLFW_KEY_0,
                                       GLFW_KEY_1,
                                       GLFW_KEY_2,
                                       GLFW_KEY_3,
                                       GLFW_KEY_4,
                                       GLFW_KEY_5,
                                       GLFW_KEY_6,
                                       GLFW_KEY_7,
                                       GLFW_KEY_8,
                                       GLFW_KEY_9,

                                       GLFW_KEY_ESCAPE,
                                       GLFW_KEY_SPACE,
                                       GLFW_KEY_ENTER,
                                       GLFW_KEY_TAB,
                                       GLFW_KEY_BACKSPACE,
                                       GLFW_KEY_LEFT_SHIFT,
                                       GLFW_KEY_LEFT_CONTROL,
                                       GLFW_KEY_LEFT,
                                       GLFW_KEY_RIGHT,
                                       GLFW_KEY_UP,
                                       GLFW_KEY_DOWN };
static int nuglfw__button_to_mouse[]
    = { GLFW_MOUSE_BUTTON_LEFT,   GLFW_MOUSE_BUTTON_RIGHT,
        GLFW_MOUSE_BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_1,
        GLFW_MOUSE_BUTTON_2,      GLFW_MOUSE_BUTTON_3,
        GLFW_MOUSE_BUTTON_4,      GLFW_MOUSE_BUTTON_5 };

#define NUGLFW_BUTTON_IS_KEY(b)   b < NUEXT_BUTTON_MOUSE_LEFT
#define NUGLFW_BUTTON_IS_MOUSE(b) b >= NUEXT_BUTTON_MOUSE_LEFT
#define NUGLFW_BUTTON_TO_KEY(b)   nuglfw__button_to_key[b]
#define NUGLFW_BUTTON_TO_MOUSE(b) \
    nuglfw__button_to_mouse[b - NUEXT_BUTTON_MOUSE_LEFT]

static void
nuglfw__dispatch_binding_button (nu_u32_t binding, nu_bool_t pressed)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        const nuglfw__binding_t *binding
            = &_ctx.platform.input.glfw.bindings.data[current];
        nu_assert(binding->input_index < _ctx.platform.input.entries.capacity);

        float value = NU_INPUT_RELEASED;
        if (pressed)
        {
            value = binding->button.pressed;
        }

        _ctx.platform.input.entries.data[binding->input_index].state.value
            = value;
        current = binding->next;
    }
}
static void
nuglfw__dispatch_binding_axis (nu_u32_t binding, float value)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        const nuglfw__binding_t *binding
            = &_ctx.platform.input.glfw.bindings.data[current];
        nu_assert(binding->input_index < _ctx.platform.input.entries.capacity);
        _ctx.platform.input.entries.data[binding->input_index].state.value
            = value * binding->axis.scale;
        current = binding->next;
    }
}
static nu_bool_t
nuglfw__find_binding (nu_u32_t binding, nu_input_t input)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        if (_ctx.platform.input.glfw.bindings.data[current].input_index
            == nu_handle_index(input))
        {
            return NU_TRUE;
        }
        current = _ctx.platform.input.glfw.bindings.data[current].next;
    }
    return NU_FALSE;
}

static void
nuglfw__update_viewport (nuglfw__viewport_t *v)
{
    nu_vec2_t global_pos  = nu_vec2(v->extent.p.x, v->extent.p.y);
    nu_vec2_t global_size = nu_vec2(v->extent.s.x, v->extent.s.y);

    float aspect_ratio = (float)v->screen.x / (float)v->screen.y;

    nu_vec2_t size = NU_VEC2_ZERO;
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
            float min      = nu_max(1.0f, nu_floor(nu_min(w_factor, h_factor)));
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
    v->viewport    = nu_rect(vpos.x, vpos.y, size.x, size.y);
}
static void
nuglfw__viewport_cursor (const nuglfw__viewport_t *v,
                         nu_vec2_t                 pos,
                         nu_vec2_t                *cursor)
{
    nu_vec2_t relpos = nu_rect_normalize(v->viewport, pos);
    *cursor = nu_vec2_mul(*cursor, nu_vec2(v->viewport.s.x, v->viewport.s.y));
}

static void
nuglfw__key_callback (
    GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key != GLFW_KEY_UNKNOWN)
    {
        if (action == GLFW_PRESS)
        {
            nuglfw__dispatch_binding_button(
                _ctx.platform.input.glfw.key_to_first_binding[key], NU_TRUE);

            if (key == NUGLFW_FULLSCREEN_KEY)
            {
                _ctx.platform.surface.glfw.switch_fullscreen = NU_TRUE;
            }
            else if (key == GLFW_KEY_ESCAPE
                     && _ctx.platform.surface.glfw.capture_mouse)
            {
                _ctx.platform.surface.glfw.switch_capture_mouse = NU_TRUE;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            nuglfw__dispatch_binding_button(
                _ctx.platform.input.glfw.key_to_first_binding[key], NU_FALSE);
        }
    }
}
static void
nuglfw__mouse_button_callback (GLFWwindow *window,
                               int         button,
                               int         action,
                               int         mode)
{
    if (action == GLFW_PRESS)
    {
        nuglfw__dispatch_binding_button(
            _ctx.platform.input.glfw.mouse_button_to_first_binding[button],
            NU_TRUE);

        if (button == GLFW_MOUSE_BUTTON_1
            && !_ctx.platform.surface.glfw.capture_mouse)
        {
            if (nu_timer_elapsed(&_ctx.platform.surface.glfw.last_mouse_click)
                < 500.0)
            {
                _ctx.platform.surface.glfw.switch_capture_mouse = NU_TRUE;
            }
            nu_timer_reset(&_ctx.platform.surface.glfw.last_mouse_click);
        }
    }
    else if (action == GLFW_RELEASE)
    {
        nuglfw__dispatch_binding_button(
            _ctx.platform.input.glfw.mouse_button_to_first_binding[button],
            NU_FALSE);
    }
}
static void
nuglfw__cursor_position_callback (GLFWwindow *window, double xpos, double ypos)
{
    _ctx.platform.input.glfw.mouse_position = nu_vec2((float)xpos, (float)ypos);
}
static void
nuglfw__character_callback (GLFWwindow *window, int codepoint)
{
    nu__platform_t *platform
        = (nu__platform_t *)glfwGetWindowUserPointer(window);
}
static void
nuglfw__mouse_scroll_callback (GLFWwindow *window,
                               double      xoffset,
                               double      yoffset)
{
    _ctx.platform.input.glfw.mouse_scroll
        = nu_vec2((float)xoffset, (float)yoffset);
}
static void
nuglfw__window_size_callback (GLFWwindow *window, int width, int height)
{
    _ctx.platform.surface.glfw.viewport.extent.s.x = width;
    _ctx.platform.surface.glfw.viewport.extent.s.y = height;
    nuglfw__update_viewport(&_ctx.platform.surface.glfw.viewport);
}

static nu_error_t
nuglfw__init (void)
{
    nuglfw__surface_t *surface = &_ctx.platform.surface.glfw;
    nuglfw__input_t   *input   = &_ctx.platform.input.glfw;
    nu_uvec2_t         size    = _ctx.platform.surface.size;

    const nu_int_t width  = NUGLFW_WINDOW_WIDTH;
    const nu_int_t height = NUGLFW_WINDOW_HEIGHT;

    // Initialize GLFW
    if (!glfwInit())
    {
        return NU_ERROR_BACKEND;
    }

    // Initialize values
    surface->fullscreen           = NU_FALSE;
    surface->switch_fullscreen    = NU_FALSE;
    surface->capture_mouse        = NU_FALSE;
    surface->switch_capture_mouse = NU_FALSE;
    nu_timer_reset(&surface->last_mouse_click);

    // Create window
    surface->win = glfwCreateWindow(width, height, "nucleus", NU_NULL, NU_NULL);
    if (!surface->win)
    {
        return NU_ERROR_BACKEND;
    }

    // Set default mouse mode
    // glfwSetInputMode(surface->win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(surface->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize viewport
    surface->viewport.mode     = NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT;
    surface->viewport.screen   = size;
    surface->viewport.extent   = nu_rect(0, 0, width, height);
    surface->viewport.viewport = nu_rect(0, 0, width, height);
    nuglfw__update_viewport(&surface->viewport);

    // Setup default cursor mode
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(surface->win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // Bind callbacks
    glfwMakeContextCurrent(surface->win);

    glfwSetKeyCallback(surface->win, nuglfw__key_callback);
    glfwSetMouseButtonCallback(surface->win, nuglfw__mouse_button_callback);
    glfwSetCursorPosCallback(surface->win, nuglfw__cursor_position_callback);
    glfwSetScrollCallback(surface->win, nuglfw__mouse_scroll_callback);
    glfwSetWindowSizeCallback(surface->win, nuglfw__window_size_callback);

    // Get initial mouse position
    double xpos, ypos;
    glfwGetCursorPos(surface->win, &xpos, &ypos);
    input->mouse_position     = nu_vec2((float)xpos, (float)ypos);
    input->mouse_old_position = input->mouse_position;

    // Initialize inputs
    nu_pool_init(10, &input->bindings);
    for (nu_u32_t i = 0; i <= GLFW_KEY_LAST; ++i)
    {
        input->key_to_first_binding[i] = NUGLFW_ID_NONE;
    }
    for (nu_u32_t i = 0; i <= GLFW_MOUSE_BUTTON_LAST; ++i)
    {
        input->mouse_button_to_first_binding[i] = NUGLFW_ID_NONE;
    }
    input->mouse_x_first_binding            = NUGLFW_ID_NONE;
    input->mouse_y_first_binding            = NUGLFW_ID_NONE;
    input->mouse_motion_x_pos_first_binding = NUGLFW_ID_NONE;
    input->mouse_motion_x_neg_first_binding = NUGLFW_ID_NONE;
    input->mouse_motion_y_pos_first_binding = NUGLFW_ID_NONE;
    input->mouse_motion_y_neg_first_binding = NUGLFW_ID_NONE;

    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__free (void)
{
    nu_pool_free(&_ctx.platform.input.glfw.bindings);
    glfwTerminate();
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__poll_events (void)
{
    if (_ctx.platform.surface.glfw.win)
    {
        // Update input states
        for (nu_size_t i = 0; i < _ctx.platform.input.entries.capacity; ++i)
        {
            if (_ctx.platform.input.entries.data[i].used)
            {
                _ctx.platform.input.entries.data[i].state.previous
                    = _ctx.platform.input.entries.data[i].state.value;
            }
        }

        // Reset mouse scroll
        _ctx.platform.input.glfw.mouse_scroll = NU_VEC2_ZERO;

        // Poll events
        glfwPollEvents();

        // Check close requested
        _ctx.platform.close_requested
            = glfwWindowShouldClose(_ctx.platform.surface.glfw.win);

        // Check capture mode
        if (_ctx.platform.surface.glfw.switch_capture_mouse)
        {
            _ctx.platform.surface.glfw.capture_mouse
                = !_ctx.platform.surface.glfw.capture_mouse;
            if (_ctx.platform.surface.glfw.capture_mouse)
            {
                glfwSetInputMode(_ctx.platform.surface.glfw.win,
                                 GLFW_CURSOR,
                                 GLFW_CURSOR_DISABLED);
            }
            else
            {
                glfwSetInputMode(_ctx.platform.surface.glfw.win,
                                 GLFW_CURSOR,
                                 GLFW_CURSOR_HIDDEN);
            }
            _ctx.platform.surface.glfw.switch_capture_mouse = NU_FALSE;
        }

        // Check fullscreen button
        if (_ctx.platform.surface.glfw.switch_fullscreen)
        {
            if (_ctx.platform.surface.glfw.fullscreen)
            {
                const GLFWvidmode *mode
                    = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(
                    _ctx.platform.surface.glfw.win,
                    NU_NULL,
                    _ctx.platform.surface.glfw.previous_position.x,
                    _ctx.platform.surface.glfw.previous_position.y,
                    _ctx.platform.surface.glfw.previous_size.x,
                    _ctx.platform.surface.glfw.previous_size.y,
                    GLFW_DONT_CARE);
            }
            else
            {
                int x, y;
                glfwGetWindowSize(_ctx.platform.surface.glfw.win, &x, &y);
                _ctx.platform.surface.glfw.previous_size = nu_uvec2(x, y);
                glfwGetWindowPos(_ctx.platform.surface.glfw.win, &x, &y);
                _ctx.platform.surface.glfw.previous_position = nu_uvec2(x, y);

                const GLFWvidmode *mode
                    = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(_ctx.platform.surface.glfw.win,
                                     glfwGetPrimaryMonitor(),
                                     0,
                                     0,
                                     mode->width,
                                     mode->height,
                                     GLFW_DONT_CARE);
            }
            _ctx.platform.surface.glfw.switch_fullscreen = NU_FALSE;
            _ctx.platform.surface.glfw.fullscreen
                = !_ctx.platform.surface.glfw.fullscreen;
        }

        // Update mouse motion
        if (NU_TRUE || _ctx.platform.surface.glfw.capture_mouse)
        {
            nu_vec2_t mouse_motion
                = nu_vec2_sub(_ctx.platform.input.glfw.mouse_position,
                              _ctx.platform.input.glfw.mouse_old_position);
            mouse_motion = nu_vec2_divs(mouse_motion, 200);
            if (mouse_motion.x != _ctx.platform.input.glfw.mouse_motion.x)
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
                nuglfw__dispatch_binding_axis(
                    _ctx.platform.input.glfw.mouse_motion_x_pos_first_binding,
                    pos_x);
                nuglfw__dispatch_binding_axis(
                    _ctx.platform.input.glfw.mouse_motion_x_neg_first_binding,
                    neg_x);
            }
            if (mouse_motion.y != _ctx.platform.input.glfw.mouse_motion.y)
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
                nuglfw__dispatch_binding_axis(
                    _ctx.platform.input.glfw.mouse_motion_y_pos_first_binding,
                    pos_y);
                nuglfw__dispatch_binding_axis(
                    _ctx.platform.input.glfw.mouse_motion_y_neg_first_binding,
                    neg_y);
            }
            _ctx.platform.input.glfw.mouse_motion = mouse_motion;

            // Update mouse position
            if (_ctx.platform.input.glfw.mouse_position.x
                    != _ctx.platform.input.glfw.mouse_old_position.x
                || _ctx.platform.input.glfw.mouse_position.y
                       != _ctx.platform.input.glfw.mouse_old_position.y)
            {
                if (nu_rect_contains(
                        _ctx.platform.surface.glfw.viewport.viewport,
                        _ctx.platform.input.glfw.mouse_position))
                {
                    nu_vec2_t relpos = nu_rect_normalize(
                        _ctx.platform.surface.glfw.viewport.viewport,
                        _ctx.platform.input.glfw.mouse_position);
                    nuglfw__dispatch_binding_axis(
                        _ctx.platform.input.glfw.mouse_x_first_binding,
                        relpos.x);
                    nuglfw__dispatch_binding_axis(
                        _ctx.platform.input.glfw.mouse_y_first_binding,
                        relpos.y);
                }
            }
            _ctx.platform.input.glfw.mouse_old_position
                = _ctx.platform.input.glfw.mouse_position;
        }
    }

    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__swap_buffers (void)
{
    if (_ctx.platform.surface.glfw.win)
    {
        glfwSwapBuffers(_ctx.platform.surface.glfw.win);
    }
    return NU_ERROR_NONE;
}

static nu_u32_t *
nuglfw__first_binding_from_button (nuext_button_t button)
{
    if (NUGLFW_BUTTON_IS_KEY(button))
    {
        nu_u32_t key = NUGLFW_BUTTON_TO_KEY(button);
        return &_ctx.platform.input.glfw.key_to_first_binding[key];
    }
    else if (NUGLFW_BUTTON_IS_MOUSE(button))
    {
        nu_u32_t mbutton = NUGLFW_BUTTON_TO_MOUSE(button);
        return &_ctx.platform.input.glfw.mouse_button_to_first_binding[mbutton];
    }
    return NU_NULL;
}
static nu_u32_t *
nuglfw__first_binding_from_axis (nuext_axis_t axis)
{
    switch (axis)
    {
        case NUEXT_AXIS_MOUSE_X:
            return &_ctx.platform.input.glfw.mouse_x_first_binding;
        case NUEXT_AXIS_MOUSE_Y:
            return &_ctx.platform.input.glfw.mouse_y_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_X_POS:
            return &_ctx.platform.input.glfw.mouse_motion_x_pos_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_X_NEG:
            return &_ctx.platform.input.glfw.mouse_motion_x_neg_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_Y_POS:
            return &_ctx.platform.input.glfw.mouse_motion_y_pos_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_Y_NEG:
            return &_ctx.platform.input.glfw.mouse_motion_y_neg_first_binding;
    }
    return NU_NULL;
}
static nuglfw__binding_t *
nuglfw__add_binding (nu_u32_t *first_binding, nu_input_t input)
{
    nu_size_t          index;
    nuglfw__binding_t *binding
        = nu_pool_add(&_ctx.platform.input.glfw.bindings, &index);
    nu_assert(nu_handle_index(input) < _ctx.platform.input.entries.capacity);
    binding->input_index = nu_handle_index(input);
    binding->next        = *first_binding;
    *first_binding       = index;
    return binding;
}
static nu_error_t
nuglfw__bind_button_value (nu_input_t     input,
                           nuext_button_t button,
                           float          pressed_value)
{
    // Check duplicated binding
    nu_u32_t *first_binding = nuglfw__first_binding_from_button(button);
    nu_assert(first_binding);
    if (nuglfw__find_binding(*first_binding, input))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nuglfw__binding_t *binding = nuglfw__add_binding(first_binding, input);
    binding->button.pressed    = pressed_value;

    return NU_ERROR_NONE;
}
static nu_error_t
nuglfw__bind_button (nu_input_t input, nuext_button_t button)
{
    return nuglfw__bind_button_value(input, button, NU_INPUT_PRESSED);
}
static nu_error_t
nuglfw__bind_axis (nu_input_t input, nuext_axis_t axis)
{
    // Check duplicated
    nu_u32_t *first_binding = nuglfw__first_binding_from_axis(axis);
    nu_assert(first_binding);
    if (nuglfw__find_binding(*first_binding, input))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nuglfw__binding_t *binding = nuglfw__add_binding(first_binding, input);
    binding->axis.scale        = 1.0;

    return NU_ERROR_NONE;
}

#endif
