#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/platform.h>

static nu_allocator_t alloc;
static nu_logger_t    logger;
static nu_platform_t  platform;

int
main (void)
{
    nu_error_t error;
    (void)error;

    nuext_stdlib_allocator_init(&alloc);

    nu_platform_info_t info;
    info.width     = 500;
    info.height    = 500;
    info.allocator = alloc;
    error          = nu_platform_init(&info, &platform);
    NU_ERROR_ASSERT(error);

    return 0;
}
