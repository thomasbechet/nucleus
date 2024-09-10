#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

int
main (int argc, char *argv[])
{
    nu_init();
    nu_info("Hello World");
    nu_terminate();
    return 0;
}
