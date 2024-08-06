#ifndef NUGLFW_IMPL_H
#define NUGLFW_IMPL_H

#define GLFW_INCLUDE_NONE
#include <nucleus/external/glfw-3.4/include/GLFW/glfw3.h>

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
nuglfw__dispatch_binding_button (nuglfw__input_t *backend,
                                 nu_u32_t         binding,
                                 nu_bool_t        pressed)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        const nuglfw__binding_t *binding = &backend->bindings[current];

        float value = NU_INPUT_RELEASED;
        if (pressed)
        {
            value = binding->button.pressed;
        }

        backend->inputs[binding->id].state.value = value;
        current                                  = binding->next;
    }
}
static void
nuglfw__dispatch_binding_axis (nuglfw__input_t *backend,
                               nu_u32_t         binding,
                               float            value)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        const nuglfw__binding_t *binding         = &backend->bindings[current];
        backend->inputs[binding->id].state.value = value * binding->axis.scale;
        current                                  = binding->next;
    }
}
static nu_bool_t
nuglfw__find_binding (nuglfw__input_t *backend, nu_u32_t binding, nu_u32_t id)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        if (backend->bindings[current].id == id)
        {
            return NU_TRUE;
        }
        current = backend->bindings[current].next;
    }
    return NU_FALSE;
}

static void
nuglfw__update_viewport (nuglfw__viewport_t *v)
{
    nu_vec2_t global_pos  = nu_vec2(v->extent.x, v->extent.y);
    nu_vec2_t global_size = nu_vec2(v->extent.w, v->extent.h);

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
    v->viewport    = nu_rect(vpos.x, vpos.y, size.x, size.y);
}
static void
nuglfw__viewport_cursor (const nuglfw__viewport_t *v,
                         nu_vec2_t                 pos,
                         nu_vec2_t                *cursor)
{
    nu_vec2_t relpos = nu_rect_normalize(v->viewport, pos);
    *cursor = nu_vec2_mul(*cursor, nu_vec2(v->viewport.w, v->viewport.h));
}

static void
nuglfw__key_callback (
    GLFWwindow *window, int key, int scancode, int action, int mode)
{
    nu_platform_t *platform = glfwGetWindowUserPointer(window);
    if (key != GLFW_KEY_UNKNOWN)
    {
        if (action == GLFW_PRESS)
        {
            nuglfw__dispatch_binding_button(
                &platform->_input.glfw,
                platform->_input.glfw.key_to_first_binding[key],
                NU_TRUE);

            if (key == NUGLFW_FULLSCREEN_KEY)
            {
                platform->_surface.glfw.switch_fullscreen = NU_TRUE;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            nuglfw__dispatch_binding_button(
                &platform->_input.glfw,
                platform->_input.glfw.key_to_first_binding[key],
                NU_FALSE);

            if (key == NUGLFW_FULLSCREEN_KEY)
            {
                platform->_surface.glfw.switch_fullscreen = NU_FALSE;
            }
        }
    }
}
static void
nuglfw__mouse_button_callback (GLFWwindow *window,
                               int         button,
                               int         action,
                               int         mode)
{
    nu_platform_t *platform = glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS)
    {
        nuglfw__dispatch_binding_button(
            &platform->_input.glfw,
            platform->_input.glfw.mouse_button_to_first_binding[button],
            NU_TRUE);
    }
    else if (action == GLFW_RELEASE)
    {
        nuglfw__dispatch_binding_button(
            &platform->_input.glfw,
            platform->_input.glfw.mouse_button_to_first_binding[button],
            NU_FALSE);
    }
}
static void
nuglfw__cursor_position_callback (GLFWwindow *window, double xpos, double ypos)
{
    nu_platform_t *platform              = glfwGetWindowUserPointer(window);
    platform->_input.glfw.mouse_position = nu_vec2((float)xpos, (float)ypos);
}
static void
nuglfw__character_callback (GLFWwindow *window, int codepoint)
{
    nu_platform_t *platform = glfwGetWindowUserPointer(window);
}
static void
nuglfw__mouse_scroll_callback (GLFWwindow *window,
                               double      xoffset,
                               double      yoffset)
{
    nu_platform_t *platform = glfwGetWindowUserPointer(window);
    platform->_input.glfw.mouse_scroll
        = nu_vec2((float)xoffset, (float)yoffset);
}
static void
nuglfw__window_size_callback (GLFWwindow *window, int width, int height)
{
    nu_platform_t *platform = glfwGetWindowUserPointer(window);
    platform->_surface.glfw.viewport.extent.w = width;
    platform->_surface.glfw.viewport.extent.h = height;
    nuglfw__update_viewport(&platform->_surface.glfw.viewport);
}

static nu_error_t
nuglfw__init (nu_platform_t *platform)
{
    nuglfw__surface_t *surface = &platform->_surface.glfw;
    nuglfw__input_t   *input   = &platform->_input.glfw;
    nu_uvec2_t         size    = platform->_surface.size;

    const nu_int_t width  = NUGLFW_WINDOW_WIDTH;
    const nu_int_t height = NUGLFW_WINDOW_HEIGHT;

    // Initialize GLFW
    if (!glfwInit())
    {
        return NU_ERROR_BACKEND;
    }

    // Initialize values
    surface->fullscreen        = NU_FALSE;
    surface->switch_fullscreen = NU_FALSE;

    // Create window
    surface->win = glfwCreateWindow(width, height, "nucleus", NU_NULL, NU_NULL);
    if (!surface->win)
    {
        return NU_ERROR_BACKEND;
    }

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
    glfwSetWindowUserPointer(surface->win,
                             platform); // TODO: ctx struct might be moved
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
    input->input_count  = 0;
    input->free_input   = 0;
    input->free_binding = 0;
    for (nu_u32_t i = 0; i < NUGLFW_MAX_INPUT; ++i)
    {
        input->inputs[i].free = i + 1;
    }
    for (nu_u32_t i = 0; i < NUGLFW_MAX_BINDING; ++i)
    {
        input->bindings[i].next = i + 1;
    }
    input->inputs[NUGLFW_MAX_INPUT - 1].free     = NUGLFW_ID_NONE;
    input->bindings[NUGLFW_MAX_BINDING - 1].next = NUGLFW_ID_NONE;
    for (nu_u32_t i = 0; i < GLFW_KEY_LAST; ++i)
    {
        input->key_to_first_binding[i] = NUGLFW_ID_NONE;
    }
    for (nu_u32_t i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
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
nuglfw__terminate (void)
{
    glfwTerminate();
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__poll_events (nuglfw__input_t   *ctx,
                     nuglfw__surface_t *surface,
                     nu_bool_t         *close_requested)
{
    if (surface->win)
    {
        // Update input states
        for (nu_size_t i = 0; i < ctx->input_count; ++i)
        {
            ctx->inputs[i].state.previous = ctx->inputs[i].state.value;
        }

        // Reset mouse scroll
        ctx->mouse_scroll = NU_VEC2_ZERO;

        // Poll events
        glfwPollEvents();

        // Check close requested
        *close_requested = glfwWindowShouldClose(surface->win);

        // Check fullscreen button
        if (surface->switch_fullscreen)
        {
            if (surface->fullscreen)
            {
                glfwSetWindowMonitor(surface->win,
                                     NULL,
                                     surface->previous_position.x,
                                     surface->previous_position.y,
                                     surface->previous_size.x,
                                     surface->previous_size.y,
                                     0);
                glfwSetInputMode(surface->win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else
            {
                int x, y;
                glfwGetWindowSize(surface->win, &x, &y);
                surface->previous_size = nu_uvec2(x, y);
                glfwGetWindowPos(surface->win, &x, &y);
                surface->previous_position = nu_uvec2(x, y);

                const GLFWvidmode *mode
                    = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                glfwSetWindowMonitor(surface->win,
                                     glfwGetPrimaryMonitor(),
                                     0,
                                     0,
                                     mode->width,
                                     mode->height,
                                     mode->refreshRate);
                glfwSetInputMode(
                    surface->win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            surface->switch_fullscreen = NU_FALSE;
            surface->fullscreen        = !surface->fullscreen;
        }

        // Update mouse motion
        nu_vec2_t mouse_motion
            = nu_vec2_sub(ctx->mouse_position, ctx->mouse_old_position);
        mouse_motion = nu_vec2_divs(mouse_motion, 1000);
        if (mouse_motion.x != ctx->mouse_motion.x)
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
                ctx, ctx->mouse_motion_x_pos_first_binding, pos_x);
            nuglfw__dispatch_binding_axis(
                ctx, ctx->mouse_motion_x_neg_first_binding, neg_x);
        }
        if (mouse_motion.y != ctx->mouse_motion.y)
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
                ctx, ctx->mouse_motion_y_pos_first_binding, pos_y);
            nuglfw__dispatch_binding_axis(
                ctx, ctx->mouse_motion_y_neg_first_binding, neg_y);
        }
        ctx->mouse_motion = mouse_motion;

        // Update mouse position
        if (ctx->mouse_position.x != ctx->mouse_old_position.x
            || ctx->mouse_position.y != ctx->mouse_old_position.y)
        {
            if (nu_rect_contains(surface->viewport.viewport,
                                 ctx->mouse_position))
            {
                nu_vec2_t relpos = nu_rect_normalize(surface->viewport.viewport,
                                                     ctx->mouse_position);
                nuglfw__dispatch_binding_axis(
                    ctx, ctx->mouse_x_first_binding, relpos.x);
                nuglfw__dispatch_binding_axis(
                    ctx, ctx->mouse_y_first_binding, relpos.y);
            }
        }
        ctx->mouse_old_position = ctx->mouse_position;
    }

    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__swap_buffers (nuglfw__surface_t *surface)
{
    if (surface->win)
    {
        glfwSwapBuffers(surface->win);
    }
    return NU_ERROR_NONE;
}

static nu_u32_t *
nuglfw__first_binding_from_button (nuglfw__input_t *ctx, nuext_button_t button)
{
    if (NUGLFW_BUTTON_IS_KEY(button))
    {
        nu_u32_t key = NUGLFW_BUTTON_TO_KEY(button);
        return &ctx->key_to_first_binding[key];
    }
    else if (NUGLFW_BUTTON_IS_MOUSE(button))
    {
        nu_u32_t mbutton = NUGLFW_BUTTON_TO_MOUSE(button);
        return &ctx->mouse_button_to_first_binding[mbutton];
    }
    return NU_NULL;
}
static nu_u32_t *
nuglfw__first_binding_from_axis (nuglfw__input_t *ctx, nuext_axis_t axis)
{
    switch (axis)
    {
        case NUEXT_AXIS_MOUSE_X:
            return &ctx->mouse_x_first_binding;
        case NUEXT_AXIS_MOUSE_Y:
            return &ctx->mouse_y_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_X_POS:
            return &ctx->mouse_motion_x_pos_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_X_NEG:
            return &ctx->mouse_motion_x_neg_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_Y_POS:
            return &ctx->mouse_motion_y_pos_first_binding;
        case NUEXT_AXIS_MOUSE_MOTION_Y_NEG:
            return &ctx->mouse_motion_y_neg_first_binding;
    }
    return NU_NULL;
}
static nu_error_t
nuglfw__create_input (nuglfw__input_t *ctx, nu_input_t *input)
{
    input->_glfwid = ctx->free_input;
    if (input->_glfwid == NUGLFW_ID_NONE)
    {
        return NU_ERROR_OUT_OF_RESOURCE;
    }
    ctx->free_input                         = ctx->inputs[input->_glfwid].free;
    ctx->inputs[input->_glfwid].state.value = NU_INPUT_RELEASED;
    ctx->inputs[input->_glfwid].state.previous = NU_INPUT_RELEASED;
    ++ctx->input_count;
    return NU_ERROR_NONE;
}
static const nu__input_state_t *
nuglfw__input_state (const nuglfw__input_t *ctx, const nu_input_t *input)
{
    return &ctx->inputs[input->_glfwid].state;
}
static nuglfw__binding_t *
nuglfw__add_binding (nuglfw__input_t *ctx,
                     nu_u32_t        *first_binding,
                     nu_input_t      *input)
{
    nu_u32_t           binding_id = ctx->free_binding;
    nuglfw__binding_t *binding    = &ctx->bindings[binding_id];
    binding->id                   = input->_glfwid;
    ctx->free_binding             = binding->next;
    binding->next                 = *first_binding;
    *first_binding                = binding_id;
    return binding;
}
static nu_error_t
nuglfw__bind_button_value (nuglfw__input_t *ctx,
                           nu_input_t      *input,
                           nuext_button_t   button,
                           float            pressed_value)
{
    NU_ASSERT(input->_glfwid != NUGLFW_ID_NONE);

    // Check duplicated binding
    nu_u32_t *first_binding = nuglfw__first_binding_from_button(ctx, button);
    NU_ASSERT(first_binding);
    if (nuglfw__find_binding(ctx, *first_binding, input->_glfwid))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nuglfw__binding_t *binding = nuglfw__add_binding(ctx, first_binding, input);
    binding->button.pressed    = pressed_value;

    return NU_ERROR_NONE;
}
static nu_error_t
nuglfw__bind_button (nuglfw__input_t *ctx,
                     nu_input_t      *input,
                     nuext_button_t   button)
{
    return nuglfw__bind_button_value(ctx, input, button, NU_INPUT_PRESSED);
}
static nu_error_t
nuglfw__bind_axis (nuglfw__input_t *ctx, nu_input_t *input, nuext_axis_t axis)
{
    NU_ASSERT(input->_glfwid != NUGLFW_ID_NONE);

    // Check duplicated
    nu_u32_t *first_binding = nuglfw__first_binding_from_axis(ctx, axis);
    NU_ASSERT(first_binding);
    if (nuglfw__find_binding(ctx, *first_binding, input->_glfwid))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nuglfw__binding_t *binding = nuglfw__add_binding(ctx, first_binding, input);
    binding->axis.scale        = 4;

    return NU_ERROR_NONE;
}

#endif
