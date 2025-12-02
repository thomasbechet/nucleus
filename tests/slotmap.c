#define NU_IMPL
#include <nucleus/vm.h>

typedef struct
{
    int value;
} object_t;

#define OBJECT_COUNT 64
#define REMOVE_COUNT 4

static nu_u32_t
nu_random ()
{
    static nu_u32_t next = 1231125;
    next                 = (unsigned int)(next * 123423542) % 23423425;
    return next;
}

int
main (void)
{

    nu__allocator_t    allocator;
    nu_allocator_api_t alloc_info;
    nu__slotmap_t      sm;
    nu__slotlist_t     sl;
    nu__slot_t         first, it;
    nu_size_t          i;
    nu__slot_t         slots[OBJECT_COUNT];
    nu__slot_t         removed[REMOVE_COUNT];
    object_t          *o;

    nu_allocator_api_stdlib(&alloc_info);
    NU_ERROR_ASSERT(nu__allocator_init(&alloc_info, &allocator));

    nu__slotmap_init(
        &allocator, NU_MEMORY_USAGE_TABLE, sizeof(object_t), OBJECT_COUNT, &sm);
    nu__slotlist_init(&allocator, NU_MEMORY_USAGE_TABLE, OBJECT_COUNT, &sl);

    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        nu__slot_t slot = nu__slotmap_add(sm);
        o               = nu__slotmap_get(sm, slot);
        o->value        = i;
        slots[i]        = slot;
    }

    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        o = nu__slotmap_get(sm, slots[i]);
        NU_ASSERT(o->value == i);
    }

    first = NU_SLOT_NULL;
    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        nu__slotlist_add_first(sl, &first, slots[i]);
    }

    it = first;
    for (i = 0; i < OBJECT_COUNT; ++i)
    {
        /* element are added in reverse */
        NU_ASSERT(it == slots[OBJECT_COUNT - i - 1]);
        it = nu__slotlist_next(sl, it);
    }

    for (i = 0; i < OBJECT_COUNT; i += 2)
    {
        nu__slotlist_remove(sl, &first, slots[i]);
    }

    it = first;
    for (i = 1; i < OBJECT_COUNT; i += 2)
    {
        NU_ASSERT(it == slots[OBJECT_COUNT - i]);
        it = nu__slotlist_next(sl, it);
    }

    for (i = 0; i < REMOVE_COUNT; ++i)
    {
        nu__slot_t r = nu_random() % OBJECT_COUNT;
        nu__slotmap_remove(sm, r);
        removed[i] = r;
    }

    for (i = 0; i < REMOVE_COUNT; ++i)
    {
        nu__slot_t slot = nu__slotmap_add(sm);
        NU_ASSERT(slot == removed[REMOVE_COUNT - i - 1]);
    }

    return 0;
}
