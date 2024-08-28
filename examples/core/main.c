#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/core.h>

static nu_allocator_t alloc;
static nu_logger_t    logger;

int
main (void)
{
    nu_logger_init(&NU_LOGGER_INFO_DEFAULT, &logger);

    nuext_allocator_init_stdlib(&alloc);

    nu_vec3_t v = NU_VEC3_ONE;
    NU_INFO(&logger, NU_VEC3_FORMAT, v.x, v.y, v.z);

    nu_u32_slotmap_t sm;
    nu_slotmap_init(&sm, &alloc, 5);
    for (nu_size_t i = 0; i < 10; ++i)
    {
        nu_slot_t slot;
        nu_slotmap_add(&sm, &alloc, &slot);
        *nu_slotmap_get(&sm, slot) = i;
        NU_INFO(&logger, "%d i=%d", slot, i);
    }

    // nu_slotmap_remove(&sm, 1);
    // nu_slotmap_remove(&sm, 5);
    nu_slotmap_remove(&sm, 7);
    NU_INFO(&logger, "start iter");
    for (nu_size_t i = 0; i < sm.capacity; ++i)
    {
        if (nu_slotmap_is_used(&sm, i))
        {
            nu_slot_t slot = i;
            nu_u32_t  val  = *nu_slotmap_get(&sm, slot);
            NU_INFO(&logger, "%d i=%d", slot, val);
        }
    }

    nu_slotmap_free(&sm, &alloc);

    nu_logger_free(&logger);

    return 0;
}
