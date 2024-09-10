#define NU_NO_PLATFORM
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef nu_pool(nu_u32_t) nu_u32_pool_t;

int
main (void)
{
    nu_init();

    nu_u32_pool_t pool;
    nu_pool_init(10, &pool);
    for (nu_size_t i = 0; i < 100; ++i)
    {
        nu_size_t index;
        nu_pool_add(&pool, &index);
        nu_info("%d", index);
    }
    nu_pool_free(&pool);

    nu_terminate();

    return 0;
}
