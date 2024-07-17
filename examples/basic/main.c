#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  640
#define HEIGHT 400

static nu_allocator_t alloc = NU_STDLIB_ALLOCATOR;
static nu_context_t   ctx;

static nu_action_t begin_draw;
static nu_action_t end_draw;
static nu_range_t  cursor_x;
static nu_range_t  cursor_y;

int
main (void)
{
    nu_error_t error;

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
    cursor_x.type        = NU_RANGE_TYPE_CLAMPED;
    cursor_x.clamped.min = 0;
    cursor_x.clamped.max = 0;
    cursor_y.type        = NU_RANGE_TYPE_CLAMPED;
    cursor_y.clamped.min = 0;
    cursor_y.clamped.max = 0;

    // Main loop
    nu_bool_t drawing = NU_FALSE;

    while (!nu_exit_requested(&ctx))
    {
        // Update inputs
        nu_update_action(&ctx, &begin_draw);
        nu_update_action(&ctx, &end_draw);
        nu_update_range(&ctx, &cursor_x);
        nu_update_range(&ctx, &cursor_y);

        // Detect drawing
        if (begin_draw.count)
        {
            drawing          = NU_TRUE;
            begin_draw.count = 0;
        }
        if (end_draw.count)
        {
            drawing          = NU_FALSE;
            end_draw.count = 0;
        }

        // Draw pixels
        (void)drawing;
    
        // Refresh surface
        nu_swap_buffers(&ctx);
    }

    nu_terminate(&ctx, &alloc);

    return 0;
}
