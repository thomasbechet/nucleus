#define NU_NO_PLATFORM
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef NU_POOL(nu_u32_t) nu_u32_pool_t;

int
main (void)
{
    nu_init();

    nu_u32_pool_t pool;
    NU_POOL_INIT(10, &pool);
    for (nu_size_t i = 0; i < 100; ++i)
    {
        nu_size_t index;
        NU_POOL_ADD(&pool, &index);
        NU_INFO("%d", index);
    }
    NU_POOL_FREE(&pool);

    nu_terminate();

    return 0;
}
