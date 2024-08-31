#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/core.h>

static nu_allocator_t alloc;
static nu_logger_t    logger;

int
main (void)
{
    nuext_allocator_create_stdlib(&alloc);

    nu_logger_info_t info;
    info.level     = NU_LOG_DEBUG;
    info.allocator = alloc;
    nu_logger_create(&info, &logger);

    {
        nu_u32_vec_t v;
        nu_vec_init(&v, alloc, 10);
        for (nu_size_t i = 0; i < 50; ++i)
        {
            *nu_vec_push(&v, alloc) = i;
        }
        for (nu_size_t i = 0; i < 50; ++i)
        {
            NU_INFO(logger, "%d", v.data[i]);
        }
    }

    nu_vec3_t v = NU_VEC3_ONE;
    NU_INFO(logger, NU_VEC3_FORMAT, v.x, v.y, v.z);

    nu_logger_delete(logger);

    return 0;
}
