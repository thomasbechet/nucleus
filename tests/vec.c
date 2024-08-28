#define NU_IMPLEMENTATION
#include <nucleus/core.h>

int
main (void)
{
    nu_allocator_t alloc;
    nuext_allocator_init_stdlib(&alloc);

    nu_u32_vec_t v;
    nu_vec_init(&v, &alloc, 10);

    return 0;
}
