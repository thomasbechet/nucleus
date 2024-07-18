#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  640
#define HEIGHT 400

static nu_allocator_t alloc;
static nu_context_t   ctx;

static nu_input_t draw;
static nu_input_t cursor_x;
static nu_input_t cursor_y;

int
main (void)
{
    nu_error_t error;
    alloc = NU_STDLIB_ALLOCATOR;

    // Create context
    nu_context_info_t cinfo = {
        .surface_api  = NU_SURFACE_API_GLFW,
        .input_api    = NU_INPUT_API_NONE,
        .graphics_api = NU_GRAPHICS_API_OPENGL,
    };
    error = nu_init(&cinfo, &alloc, &ctx);
    NU_ERROR_ASSERT(error);

    // Create surface
    nu_surface_info_t sinfo = { .width = WIDTH, .height = HEIGHT };
    error                   = nu_open_surface(&ctx, &sinfo, &alloc);
    NU_ERROR_ASSERT(error);

    // Configure inputs
    draw     = NU_INPUT(1);
    cursor_x = NU_INPUT(WIDTH);
    cursor_y = NU_INPUT(HEIGHT);

    // Main loop
    nu_bool_t drawing = NU_FALSE;

    while (!nu_exit_requested(&ctx))
    {
        // Poll events
        nu_poll_events(&ctx);

        // Update inputs
        nu_update_inputs(
            &ctx, (nu_input_t *[]) { &draw, &cursor_x, &cursor_y }, 3);

        // Detect drawing
        if (nu_input_changed(&cursor_x) || nu_input_changed(&cursor_y))
        {
        }

        // Draw pixels
        (void)drawing;

        // Refresh surface
        nu_swap_buffers(&ctx);
    }

    nu_terminate(&ctx, &alloc);

    return 0;
}
