#define NU_NO_GRAPHICS
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

static nu_input_t exit_input;

int
main (void)
{
    nu_init();

    exit_input = nu_input_create();
    nuext_input_bind_button(exit_input, NUEXT_BUTTON_ESCAPE);

    nu_bool_t running = NU_TRUE;
    while (!nu_exit_requested() && running)
    {
        if (nu_input_just_pressed(exit_input))
        {
            running = NU_FALSE;
        }
        nu_poll_events();
        nu_swap_buffers();
    }

    nu_terminate();

    return 0;
}
