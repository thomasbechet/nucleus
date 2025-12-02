#define NU_IMPL
#include <nucleus/vm.h>

typedef struct
{
    int value;
} object_t;

#define OBJECT_COUNT 128

int
main (void)
{
    nu__allocator_t    allocator;
    nu_allocator_api_t alloc_info;
    nu__list_t         list;
    object_t          *objs[OBJECT_COUNT];
    object_t          *it;
    nu_size_t          i;

    nu_allocator_api_stdlib(&alloc_info);

    NU_ERROR_ASSERT(nu__allocator_init(&alloc_info, &allocator));
    nu__list_init(&list, sizeof(object_t));

    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        objs[i] = nu__list_append(&list, &allocator, NU_MEMORY_USAGE_CORE);
        NU_ASSERT(objs[i]);
        objs[i]->value = i;
    }

    it = nu__list_first(&list);
    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        NU_ASSERT(it);
        NU_ASSERT(it->value == i);
        it = nu__list_next(it);
    }

    for (i = 0; i < OBJECT_COUNT; i += 2)
    {
        nu__list_remove(&list, objs[i]);
    }

    it = nu__list_first(&list);
    for (i = 1; i < OBJECT_COUNT; i += 2)
    {
        NU_ASSERT(it);
        NU_ASSERT(it->value == i);
        it = nu__list_next(it);
    }

    for (i = 0; i < OBJECT_COUNT; i += 2)
    {
        nu__list_remove(&list, objs[i]);
    }

    return 0;
}
