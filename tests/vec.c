#define NU_STDLIB
#define NU_NO_PLATFORM
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

typedef struct
{
    nu_u32_t val;
    nu_u8_t  a;
} mystruct;

typedef nu_vec(mystruct) mystruct_vec_t;

int
main (void)
{
    nu_init();

    mystruct_vec_t v;
    nu_vec_init(1, &v);

    for (nu_size_t i = 0; i < 100; ++i)
    {
        (void)nu_vec_push(&v);
        nu_vec_last(&v)->val = i;
    }
    for (nu_size_t i = 0; i < 100; ++i)
    {
        nu_assert(v.data[i].val == i);
    }
    nu_assert(nu_vec_last(&v)->val == 99);
    nu_assert(nu_vec_pop(&v)->val == 99);
    nu_assert(nu_vec_last(&v)->val == 98);
    nu_assert(v.size == 99);

    return 0;
}
