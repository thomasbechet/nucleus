#include <iostream>

#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

int
main (int argc, char *argv[])
{
    nu_initialize();
    NU_INFO("Hello World");
    nu_terminate();
    return 0;
}
