#include <stdio.h>
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
static nu_input_t quit;

int
main (void)
{
    nu_error_t error;

    // Create allocator
    nuext_init_stdlib_allocator(&alloc);

    // Create context
    nu_context_info_t cinfo;
    nu_context_info_default(&cinfo);
    cinfo.width    = WIDTH;
    cinfo.height   = HEIGHT;
    cinfo.renderer = NU_RENDERER_GL;
    error          = nu_init(&cinfo, &alloc, &ctx);
    NU_ERROR_ASSERT(error);

    // Configure inputs
    nu_init_input(&draw);
    nu_init_input(&cursor_x);
    nu_init_input(&cursor_y);
    nu_init_input(&quit);

    // Bind inputs
    error = nuext_bind_button(&ctx, &draw, NUEXT_BUTTON_W);
    NU_ERROR_ASSERT(error);
    error = nuext_bind_button(&ctx, &draw, NUEXT_BUTTON_MOUSE_LEFT);
    NU_ERROR_ASSERT(error);
    error = nuext_bind_button(&ctx, &quit, NUEXT_BUTTON_ESCAPE);
    NU_ERROR_ASSERT(error);

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    while (!nu_exit_requested(&ctx) && running)
    {
        // Poll events
        nu_poll_events(&ctx);

        // Update inputs
        nu_update_inputs(
            &ctx, (nu_input_t *[]) { &draw, &cursor_x, &cursor_y, &quit }, 4);

        // Detect drawing
        if (nu_input_changed(&cursor_x) || nu_input_changed(&cursor_y))
        {
        }

        if (nu_input_pressed(&draw))
        {
            printf("pressed\n");
        }
        if (nu_input_just_pressed(&draw))
        {
            printf("just pressed\n");
        }
        if (nu_input_just_released(&draw))
        {
            printf("just released\n");
        }

        if (nu_input_just_pressed(&quit))
        {
            running = NU_FALSE;
        }

        // Draw pixels
        (void)drawing;

        nu_clear(&ctx);
        nu_render(&ctx);

        // Refresh surface
        nu_swap_buffers(&ctx);
    }

    nu_terminate(&ctx, &alloc);

    return 0;
}
