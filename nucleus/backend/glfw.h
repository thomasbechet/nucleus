#ifndef NU_BACKEND_GLFW_H
#define NU_BACKEND_GLFW_H

#include <nucleus/types.h>

#ifdef NU_IMPLEMENTATION

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
nuglfw__dispatch_binding (nuglfw__backend_t *backend,
                          nu_u32_t           binding,
                          float              value)
{
    nu_u32_t current = binding;
    while (current != NUGLFW_ID_NONE)
    {
        backend->inputs[backend->bindings[current].id].value = value;
        current = backend->bindings[current].next;
    }
}
static nu_bool_t
nuglfw__find_binding (nuglfw__backend_t *backend, nu_u32_t binding, nu_u32_t id)
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
nuglfw__key_callback (
    GLFWwindow *window, int key, int scancode, int action, int mode)
{
    nuglfw__backend_t *backend = glfwGetWindowUserPointer(window);
    if (key != GLFW_KEY_UNKNOWN)
    {
        if (action == GLFW_PRESS)
        {
            nuglfw__dispatch_binding(
                backend, backend->key_to_first_binding[key], NU_INPUT_PRESSED);
        }
        else if (action == GLFW_RELEASE)
        {
            nuglfw__dispatch_binding(
                backend, backend->key_to_first_binding[key], NU_INPUT_RELEASED);
        }
    }
}
static void
nuglfw__mouse_button_callback (GLFWwindow *window,
                               int         button,
                               int         action,
                               int         mode)
{
    nuglfw__backend_t *backend = glfwGetWindowUserPointer(window);
    if (action == GLFW_PRESS)
    {
        nuglfw__dispatch_binding(backend,
                                 backend->mouse_button_to_first_binding[button],
                                 NU_INPUT_PRESSED);
    }
    else if (action == GLFW_RELEASE)
    {
        nuglfw__dispatch_binding(backend,
                                 backend->mouse_button_to_first_binding[button],
                                 NU_INPUT_RELEASED);
    }
}
static void
nuglfw__cursor_position_callback (GLFWwindow *window, double xpos, double ypos)
{
    nuglfw__backend_t *backend = glfwGetWindowUserPointer(window);
    backend->mouse_position[0] = (float)xpos;
    backend->mouse_position[1] = (float)ypos;
}
static void
nuglfw__character_callback (GLFWwindow *window, int codepoint)
{
    nuglfw__backend_t *backend = glfwGetWindowUserPointer(window);
}
static void
nuglfw__mouse_scroll_callback (GLFWwindow *window,
                               double      xoffset,
                               double      yoffset)
{
    nuglfw__backend_t *backend = glfwGetWindowUserPointer(window);
    backend->mouse_scroll[0]   = (float)xoffset;
    backend->mouse_scroll[1]   = (float)yoffset;
}

static nu_error_t
nuglfw__init (nu_context_t *ctx, const nu_context_info_t *info)
{
    NU_UNUSED(info);

    // Initialize GLFW
    if (!glfwInit())
    {
        return NU_ERROR_BACKEND;
    }

    // Create window
    ctx->_glfw.win = glfwCreateWindow(
        ctx->_info.width, ctx->_info.height, "", NU_NULL, NU_NULL);
    if (!ctx->_glfw.win)
    {
        return NU_ERROR_BACKEND;
    }

    // Setup default cursor mode
    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(ctx->_glfw.win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // Bind callbacks
    glfwSetWindowUserPointer(ctx->_glfw.win,
                             &ctx->_glfw); // TODO: ctx struct might be moved
    glfwMakeContextCurrent(ctx->_glfw.win);

    glfwSetKeyCallback(ctx->_glfw.win, nuglfw__key_callback);
    glfwSetMouseButtonCallback(ctx->_glfw.win, nuglfw__mouse_button_callback);
    glfwSetCursorPosCallback(ctx->_glfw.win, nuglfw__cursor_position_callback);
    glfwSetScrollCallback(ctx->_glfw.win, nuglfw__mouse_scroll_callback);

    // Get initial mouse position
    double xpos, ypos;
    glfwGetCursorPos(ctx->_glfw.win, &xpos, &ypos);
    ctx->_glfw.mouse_position[0] = (float)xpos;
    ctx->_glfw.mouse_position[1] = (float)ypos;
    nu_vec2_copy(ctx->_glfw.mouse_position, ctx->_glfw.mouse_old_position);

    // Initialize inputs
    ctx->_glfw.free_input   = 0;
    ctx->_glfw.free_binding = 0;
    for (nu_u32_t i = 0; i < NUGLFW_MAX_INPUT; ++i)
    {
        ctx->_glfw.inputs[i].free = i + 1;
    }
    for (nu_u32_t i = 0; i < NUGLFW_MAX_BINDING; ++i)
    {
        ctx->_glfw.bindings[i].next = i + 1;
    }
    ctx->_glfw.inputs[NUGLFW_MAX_INPUT - 1].free     = NUGLFW_ID_NONE;
    ctx->_glfw.bindings[NUGLFW_MAX_BINDING - 1].next = NUGLFW_ID_NONE;
    for (nu_u32_t i = 0; i < GLFW_KEY_LAST; ++i)
    {
        ctx->_glfw.key_to_first_binding[i] = NUGLFW_ID_NONE;
    }
    for (nu_u32_t i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
    {
        ctx->_glfw.mouse_button_to_first_binding[i] = NUGLFW_ID_NONE;
    }

    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__terminate (nuglfw__backend_t *backend)
{
    NU_UNUSED(backend);
    glfwTerminate();
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__poll_events (nu_context_t *ctx)
{
    if (ctx->_glfw.win)
    {
        // Reset mouse scroll
        nu_vec2_zero(ctx->_glfw.mouse_scroll);
        // Poll events
        glfwPollEvents();
        // Check close requested
        ctx->_close_requested = glfwWindowShouldClose(ctx->_glfw.win);
        // Update mouse motion
        nu_vec2_sub(ctx->_glfw.mouse_position,
                    ctx->_glfw.mouse_old_position,
                    ctx->_glfw.mouse_motion);
        nu_vec2_copy(ctx->_glfw.mouse_position, ctx->_glfw.mouse_old_position);
    }
    return NU_ERROR_NONE;
}

static nu_error_t
nuglfw__swap_buffers (nu_context_t *ctx)
{
    if (ctx->_glfw.win)
    {
        glfwSwapBuffers(ctx->_glfw.win);
    }
    return NU_ERROR_NONE;
}

static nu_u32_t *
nuglfw__first_binding_from_button (nu_context_t *ctx, nuext_button_t button)
{
    if (NUGLFW_BUTTON_IS_KEY(button))
    {
        nu_u32_t key = NUGLFW_BUTTON_TO_KEY(button);
        return &ctx->_glfw.key_to_first_binding[key];
    }
    else if (NUGLFW_BUTTON_IS_MOUSE(button))
    {
        nu_u32_t mbutton = NUGLFW_BUTTON_TO_MOUSE(button);
        return &ctx->_glfw.mouse_button_to_first_binding[mbutton];
    }
    return NU_NULL;
}
static nu_error_t
nuglfw__bind_button (nu_context_t  *ctx,
                     nu_input_t    *input,
                     nuext_button_t button)
{
    // Insert input if needed
    if (input->_glfwid == NUGLFW_ID_NONE)
    {
        input->_glfwid = ctx->_glfw.free_input;
        if (input->_glfwid == NUGLFW_ID_NONE)
        {
            return NU_ERROR_OUT_OF_RESOURCE;
        }
        ctx->_glfw.free_input = ctx->_glfw.inputs[input->_glfwid].free;
        ctx->_glfw.inputs[input->_glfwid].value = NU_INPUT_RELEASED;
    }

    // Check duplicated binding
    nu_u32_t *first_binding = nuglfw__first_binding_from_button(ctx, button);
    NU_ASSERT(first_binding);
    if (nuglfw__find_binding(&ctx->_glfw, *first_binding, input->_glfwid))
    {
        return NU_ERROR_DUPLICATED;
    }

    // Add binding
    nu_u32_t           binding_id = ctx->_glfw.free_binding;
    nuglfw__binding_t *binding    = &ctx->_glfw.bindings[binding_id];
    binding->id                   = input->_glfwid;
    ctx->_glfw.free_binding       = binding->next;

    // Insert binding to button list
    binding->next  = *first_binding;
    *first_binding = binding_id;

    return NU_ERROR_NONE;
}

static void
nuglfw__update_input (nu_context_t *ctx, nu_input_t *input)
{
    if (input->_glfwid != NUGLFW_ID_NONE)
    {
        input->_value = ctx->_glfw.inputs[input->_glfwid].value;
    }
}

#endif

#endif
