#define NU_NO_PLATFORM
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef nu_pool(nu_u32_t) nu_u32_pool_t;

int
main (void)
{
    nu_initialize();

    NU_INFO("Hello World");

    nu_u32_pool_t pool;
    nu_pool_init(10, &pool);
    nu_size_t index;
    nu_u32_t *a = nu_pool_add(&pool, &index);
    NU_INFO("%d", index);

    nu_terminate();

    return 0;
}
