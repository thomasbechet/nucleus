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

    // Create allocator
    nuext_init_stdlib_allocator(&alloc);

    // Create context
    nu_context_info_t cinfo;
    nu_context_info_default(&cinfo);
    cinfo.width  = WIDTH;
    cinfo.height = HEIGHT;
    error        = nu_init(&cinfo, &alloc, &ctx);
    NU_ERROR_ASSERT(error);

    // Configure inputs
    nu_init_input(&draw);
    nu_init_input(&cursor_x);
    nu_init_input(&cursor_y);

    // Bind inputs

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
