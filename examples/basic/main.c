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
    error = nu_create_input(&ctx, &draw);
    NU_ERROR_ASSERT(error);
    error = nu_create_input(&ctx, &cursor_x);
    NU_ERROR_ASSERT(error);
    error = nu_create_input(&ctx, &cursor_y);
    NU_ERROR_ASSERT(error);
    error = nu_create_input(&ctx, &quit);
    NU_ERROR_ASSERT(error);

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

        // Detect drawing
        if (nu_input_changed(&ctx, &cursor_x)
            || nu_input_changed(&ctx, &cursor_y))
        {
        }

        if (nu_input_pressed(&ctx, &draw))
        {
            printf("pressed\n");
        }
        if (nu_input_just_pressed(&ctx, &draw))
        {
            printf("just pressed\n");
        }
        if (nu_input_just_released(&ctx, &draw))
        {
            printf("just released\n");
        }

        if (nu_input_just_pressed(&ctx, &quit))
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
