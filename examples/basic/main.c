#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define NU_IMPL
#define NU_DEBUG
#define NU_STDLIB
#include <nucleus/vm.h>

static nu_size_t mem_total = 0;

void *
allocator_callback (nu_size_t         size,
                    nu_size_t         align,
                    nu_memory_usage_t usage,
                    void             *userdata)
{
    (void)align;
    (void)userdata;
    mem_total += size;
    printf("alloc %ld from %d (total: %ld)\n", size, usage, mem_total);
    return malloc(size);
}

static nu_error_t
load_vm_properties (void *userdata, nu_vm_properties_t *properties)
{
    (void)userdata;
    nu_vm_properties_default(properties);
    return NU_ERROR_NONE;
}

static void
register_archetypes (nu_vm_t vm)
{
    nu_error_t      error;
    nu_archetype_t  archetype;
    nu_field_info_t fields[] = { { "position", NU_PRIMITIVE_INT, NU_NULL },
                                 { "rotation", NU_PRIMITIVE_INT, NU_NULL } };

    error = nu_vm_create_archetype(vm, "player", fields, 2, &archetype);
    NU_ERROR_ASSERT(error);
}

static nu_error_t
cartridge_load (void *userdata, nu_cartdata_type_t type, void *data)
{
    (void)userdata;
    switch (type)
    {
        case NU_CARTDATA_VM_PROPERTIES: {
            nu_cartdata_vm_properties_t *pdata = data;
            nu_vm_properties_default(pdata->props);
            break;
        }
        case NU_CARTDATA_BUNDLE:
            break;
        case NU_CARTDATA_RESOURCE:
            break;
        case NU_CARTDATA_UNKNOWN:
            return NU_ERROR_RESOURCE_NOT_FOUND;
            break;
    }
    return NU_ERROR_NONE;
}

int
main (void)
{
    nu_vm_t      vm;
    nu_vm_info_t info;
    nu_error_t   error;
    nu_size_t    tick;

    {
        nu_fix_t a = nu_itof(123);
        nu_fix_t b = nu_itof(32);
        nu_fix_t c = nu_fmul(a, b);
        (void)a;
        (void)b;
        (void)c;
        c = b;
    }

    info.allocator.userdata = NU_NULL;
    info.allocator.callback = allocator_callback;
    info.cartridge.userdata = NU_NULL;
    info.cartridge.load     = cartridge_load;

    error = nu_vm_init(&info, &vm);
    NU_ERROR_CHECK(error, return 123);

    tick = 10;
    while (--tick)
    {
        nu_vm_tick(vm);
        usleep(16000);
    }

    nu_vm_free(vm);

    return 0;
}
