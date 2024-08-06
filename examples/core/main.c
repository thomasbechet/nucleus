#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/core.h>

static nu_allocator_t alloc;
static nu_logger_t    logger;

int
main (void)
{
    nu_logger_create(&logger);

    nu_vec3_t v = NU_VEC3_ONE;
    NU_INFO(&logger, NU_VEC3_FORMAT, v.x, v.y, v.z);

    nu_logger_delete(&logger);

    return 0;
}
