#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/core.h>

typedef struct
{
    nu_u32_t val;
    nu_u8_t  a;
} mystruct;

typedef nu_vec(mystruct) mystruct_vec_t;

int
main (void)
{
    nu_allocator_t alloc;
    nuext_allocator_init_stdlib(&alloc);

    mystruct_vec_t v;
    nu_vec_init(&v, &alloc, 1);

    for (nu_size_t i = 0; i < 100; ++i)
    {
        (void)nu_vec_push(&v, &alloc);
        nu_vec_last(&v)->val = i;
    }
    for (nu_size_t i = 0; i < 100; ++i)
    {
        NU_ASSERT(v.data[i].val == i);
    }
    NU_ASSERT(nu_vec_last(&v)->val == 99);
    NU_ASSERT(nu_vec_pop(&v)->val == 99);
    NU_ASSERT(nu_vec_last(&v)->val == 98);
    NU_ASSERT(v.size == 99);

    return 0;
}
