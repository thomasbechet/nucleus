#ifndef NU_SURFACE_H
#define NU_SURFACE_H

#include <nucleus/platform/api.h>

#ifdef NU_BUILD_GLFW

#define NUGLFW_ID_NONE 0xFFFFFFFF

#define NUGLFW_WINDOW_WIDTH  1280
#define NUGLFW_WINDOW_HEIGHT 720

#define NUGLFW_FULLSCREEN_KEY GLFW_KEY_F11

typedef struct
{
    nuext_viewport_mode_t mode;
    float                 scale_factor;
    nu_uvec2_t            screen;
    nu_rect_t             extent;
    nu_rect_t             viewport;
} nuglfw__viewport_t;

typedef struct GLFWwindow *NUGLFWwindow;

typedef struct
{
    NUGLFWwindow       win;
    nuglfw__viewport_t viewport;
    nu_bool_t          fullscreen;
    nu_bool_t          switch_fullscreen;
    nu_bool_t          capture_mouse;
    nu_uvec2_t         previous_size;
    nu_uvec2_t         previous_position;
    nu_timer_t         last_mouse_click;
    nu_bool_t          switch_capture_mouse;
} nuglfw__surface_t;

#endif

typedef struct
{
#ifdef NU_BUILD_GLFW
    nuglfw__surface_t glfw;
#endif
    nu_uvec2_t size;
} nu__surface_t;

#endif
