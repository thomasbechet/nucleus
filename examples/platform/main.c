#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/platform.h>

static nu_allocator_t    alloc;
static nu_logger_t       logger;
static nu_platform_t     platform;
static nu_input_handle_t exit_input;

int
main (void)
{
    nu_error_t error;
    (void)error;

    nuext_allocator_init_stdlib(&alloc);

    nu_platform_info_t info;
    info.width  = 500;
    info.height = 500;
    error       = nu_platform_init(&info, &alloc, &platform);
    NU_ERROR_ASSERT(error);

    error = nu_input_create(&platform, &exit_input);
    NU_ERROR_ASSERT(error);

    nuext_input_bind_button(&platform, exit_input, NUEXT_BUTTON_ESCAPE);

    while (!nu_platform_exit_requested(&platform))
    {
        if (nu_input_just_pressed(&platform, exit_input))
        {
            break;
        }
        nu_platform_poll_events(&platform);
        nu_platform_swap_buffers(&platform);
    }

    nu_platform_free(&platform);

    return 0;
}
