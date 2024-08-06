#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/graphics.h>

static nu_allocator_t  alloc;
static nu_logger_t     logger;
static nu_platform_t   platform;
static nu_renderer_t   renderer;
static nu_renderpass_t renderpass;
static nu_input_t      exit_input;

int
main (void)
{
    nu_error_t error;
    (void)error;

    nuext_stdlib_allocator_init(&alloc);

    {
        nu_platform_info_t info;
        info.width     = 500;
        info.height    = 500;
        info.allocator = alloc;
        error          = nu_platform_init(&info, &platform);
        NU_ERROR_ASSERT(error);
    }

    {
        nu_renderer_info_t info;
        info.api = NU_RENDERER_GL;
        error    = nu_renderer_init(&platform, &info, &renderer);
        NU_ERROR_ASSERT(error);
    }

    {
        nu_renderpass_info_t info;
        info.type = NU_RENDERPASS_FLAT;
        error     = nu_renderpass_create(&renderer, &info, &renderpass);
        NU_ERROR_ASSERT(error);
    }

    {
        error = nu_input_create(&platform, &exit_input);
        NU_ERROR_ASSERT(error);
    }

    nuext_input_bind_button(&platform, &exit_input, NUEXT_BUTTON_ESCAPE);

    while (!nu_exit_requested(&platform))
    {
        if (nu_input_just_pressed(&platform, &exit_input))
        {
            break;
        }
        nu_poll_events(&platform);
        nu_renderpass_submit_t submit;
        submit.flat.camera      = NU_NULL;
        submit.flat.clear_color = NU_COLOR_RED;
        submit.flat.color_target
            = nu_surface_color_target(&platform, &renderer);
        submit.flat.depth_target = NU_NULL;
        nu_renderpass_submit(&renderer, &renderpass, &submit);
        nu_render(&platform, &renderer);
        nu_swap_buffers(&platform);
    }

    nu_renderer_terminate(&renderer);
    nu_platform_terminate(&platform);

    return 0;
}
