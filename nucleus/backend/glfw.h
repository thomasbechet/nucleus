#ifndef NUGLFW_BACKEND_H
#define NUGLFW_BACKEND_H

#include <nucleus/math.h>

#if defined(NU_IMPLEMENTATION) && defined(NU_BUILD_GLFW)

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
nuglfw__dispatch_binding (nuglfw__input_t *backend,
                          nu_u32_t         binding,
                          float            value)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        backend->inputs[backend->bindings[current].id].state.value = value;
        current = backend->bindings[current].next;
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
    nu_vec2_t global_pos
        = nu_vec2_floor(nu_vec2((float)v->extent.x, (float)v->extent.y));

    nu_vec2_t global_size
        = nu_vec2_floor(nu_vec2((float)v->extent.z, (float)v->extent.w));

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
    vpos           = nu_vec2_divs(vpos, 2.0f);
    vpos           = nu_vec2_add(vpos, global_pos);
    v->viewport    = nu_vec4(vpos.x, vpos.y, size.x, size.y);
}
static void
nuglfw__viewport_cursor (const nuglfw__viewport_t *v,
                         nu_vec2_t                 pos,
                         nu_vec2_t                *cursor)
{
    nu_vec2_t relpos = nu_vec2_sub(pos, nu_vec4_xy(v->viewport));
    *cursor          = nu_vec2_div(relpos, nu_vec4_zw(v->viewport));
    *cursor          = nu_vec2_mul(*cursor, nu_vec4_zw(v->viewport));
}

static void
nuglfw__key_callback (
    GLFWwindow *window, int key, int scancode, int action, int mode)
{
    nu_context_t *ctx = glfwGetWindowUserPointer(window);
    if (key != GLFW_KEY_UNKNOWN)
    {
        if (action == GLFW_PRESS)
        {
            nuglfw__dispatch_binding(&ctx->_glfw_input,
                                     ctx->_glfw_input.key_to_first_binding[key],
                                     NU_INPUT_PRESSED);
        }
        else if (action == GLFW_RELEASE)
        {
            nuglfw__dispatch_binding(&ctx->_glfw_input,
                                     ctx->_glfw_input.key_to_first_binding[key],
                                     NU_INPUT_RELEASED);
        }
    }
}
static void
nuglfw__mouse_button_callback (GLFWwindow *window,
                               int         button,
                               int         action,
                               int         mode)
{
    nu_context_t *ctx = glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS)
    {
        nuglfw__dispatch_binding(
            &ctx->_glfw_input,
            ctx->_glfw_input.mouse_button_to_first_binding[button],
            NU_INPUT_PRESSED);
    }
    else if (action == GLFW_RELEASE)
    {
        nuglfw__dispatch_binding(
            &ctx->_glfw_input,
            ctx->_glfw_input.mouse_button_to_first_binding[button],
            NU_INPUT_RELEASED);
    }
}
static void
nuglfw__cursor_position_callback (GLFWwindow *window, double xpos, double ypos)
{
    nu_context_t *ctx               = glfwGetWindowUserPointer(window);
    ctx->_glfw_input.mouse_position = nu_vec2((float)xpos, (float)ypos);
}
static void
nuglfw__character_callback (GLFWwindow *window, int codepoint)
{
    nu_context_t *ctx = glfwGetWindowUserPointer(window);
}
static void
nuglfw__mouse_scroll_callback (GLFWwindow *window,
                               double      xoffset,
                               double      yoffset)
{
    nu_context_t *ctx             = glfwGetWindowUserPointer(window);
    ctx->_glfw_input.mouse_scroll = nu_vec2((float)xoffset, (float)yoffset);
}
static void
nuglfw__window_size_callback (GLFWwindow *window, int width, int height)
{
    nu_context_t *ctx                    = glfwGetWindowUserPointer(window);
    ctx->_glfw_surface.viewport.extent.z = width;
    ctx->_glfw_surface.viewport.extent.w = height;
    nuglfw__update_viewport(&ctx->_glfw_surface.viewport);
}

static nu_error_t
nuglfw__init (nu_context_t *ctx)
{
    nuglfw__surface_t *surface = &ctx->_glfw_surface;
    nuglfw__input_t   *input   = &ctx->_glfw_input;
    nu_uvec2_t         size    = ctx->_surface_size;

    const nu_int_t width  = NUGLFW_WINDOW_WIDTH;
    const nu_int_t height = NUGLFW_WINDOW_HEIGHT;

    // Initialize GLFW
    if (!glfwInit())
    {
        return NU_ERROR_BACKEND;
    }

    // Create window
    surface->win = glfwCreateWindow(width, height, "nucleus", NU_NULL, NU_NULL);
    if (!surface->win)
    {
        return NU_ERROR_BACKEND;
    }

    // Initialize viewport
    surface->viewport.mode     = NUEXT_VIEWPORT_STRETCH_KEEP_ASPECT;
    surface->viewport.screen   = size;
    surface->viewport.extent   = nu_uvec4(0, 0, width, height);
    surface->viewport.viewport = NU_VEC4_ZERO;
    nuglfw__update_viewport(&surface->viewport);

    // Setup default cursor mode
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(surface->win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // Bind callbacks
    glfwSetWindowUserPointer(surface->win,
                             ctx); // TODO: ctx struct might be moved
    glfwMakeContextCurrent(surface->win);

    glfwSetKeyCallback(surface->win, nuglfw__key_callback);
    glfwSetMouseButtonCallback(surface->win, nuglfw__mouse_button_callback);
    glfwSetCursorPosCallback(surface->win, nuglfw__cursor_position_callback);
    glfwSetScrollCallback(surface->win, nuglfw__mouse_scroll_callback);
    glfwSetWindowSizeCallback(surface->win, nuglfw__window_size_callback);

    // Glad initialization
    if (!gladLoadGL(glfwGetProcAddress))
    {
        return NU_ERROR_BACKEND;
    }

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
        // Update mouse motion
        ctx->mouse_motion
            = nu_vec2_sub(ctx->mouse_position, ctx->mouse_old_position);
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
static nu_error_t
nuglfw__bind_button (nuglfw__input_t *ctx,
                     nu_input_t      *input,
                     nuext_button_t   button)
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
    nu_u32_t           binding_id = ctx->free_binding;
    nuglfw__binding_t *binding    = &ctx->bindings[binding_id];
    binding->id                   = input->_glfwid;
    ctx->free_binding             = binding->next;

    // Insert binding to button list
    binding->next  = *first_binding;
    *first_binding = binding_id;

    return NU_ERROR_NONE;
}

#endif

#endif
