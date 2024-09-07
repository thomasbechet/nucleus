#define NU_NO_GRAPHICS
#define NU_NO_UTILS
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

static nu_input_t exit_input;

int
main (void)
{
    nu_initialize();

    // nu_input_create(&exit_input);
    // nuext_input_bind_button(exit_input, NUEXT_BUTTON_ESCAPE);

    nu_input_t q = nu_input_create();
    nuext_input_bind_axis(q, NUEXT_AXIS_MOUSE_X);
    nu_input_t w = nu_input_create();
    nuext_input_bind_axis(w, NUEXT_AXIS_MOUSE_Y);

    while (!nu_exit_requested())
    {
        nu_info("%lf", nu_input_value(q));
        nu_info("%lf", nu_input_value(w));
        // if (nu_input_just_pressed(exit_input))
        // {
        //     break;
        // }
        nu_poll_events();
        nu_swap_buffers();
    }

    nu_terminate();

    return 0;
}
