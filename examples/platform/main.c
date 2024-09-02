#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

static nu_input_t exit_input;

int
main (void)
{
    nu_initialize();

    nu_input_create(&exit_input);
    nuext_input_bind_button(exit_input, NUEXT_BUTTON_ESCAPE);

    while (!nu_exit_requested())
    {
        if (nu_input_just_pressed(exit_input))
        {
            break;
        }
        nu_poll_events();
        nu_swap_buffers();
    }

    nu_terminate();

    return 0;
}
