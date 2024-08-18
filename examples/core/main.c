#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/core.h>

static nu_allocator_t alloc;
static nu_logger_t    logger;

int
main (void)
{
    nu_logger_info_t info = nu_logger_info_default();
    nu_logger_init(&info, &logger);

    nuext_allocator_init_stdlib(&alloc);

    nu_vec3_t v = NU_VEC3_ONE;
    NU_INFO(&logger, NU_VEC3_FORMAT, v.x, v.y, v.z);

    nu_u32_slotmap_t sm;
    nu_slotmap_init(&sm, &alloc, 123);
    nu_slotmap_free(&sm, &alloc);

    nu_logger_terminate(&logger);

    return 0;
}
