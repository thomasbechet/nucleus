#ifndef NU_VM_H
#define NU_VM_H

#include <nucleus/vm/property.h>
#include <nucleus/vm/error.h>
#include <nucleus/vm/platform.h>
#include <nucleus/vm/table.h>
#include <nucleus/vm/allocator.h>
#include <nucleus/vm/renderer.h>
#include <nucleus/vm/cartridge.h>
#include <nucleus/vm/list.h>
#include <nucleus/vm/slotmap.h>
#include <nucleus/vm/interpreter.h>

typedef struct nu__vm *nu_vm_t;

typedef struct
{
    nu_allocator_api_t allocator;
    nu_cartridge_api_t cartridge;
} nu_vm_info_t;

NU_API nu_error_t nu_vm_init(const nu_vm_info_t *info, nu_vm_t *vm);
NU_API nu_error_t nu_vm_free(nu_vm_t vm);

NU_API nu_error_t nu_vm_tick(nu_vm_t vm);

NU_API nu_error_t nu_vm_save(nu_vm_t vm);
NU_API nu_error_t nu_vm_load(nu_vm_t vm);

NU_API nu_error_t nu_vm_bind_renderer(nu_vm_t                   vm,
                                      const nu_renderer_info_t *info);

NU_API nu_error_t nu_vm_create_archetype(nu_vm_t                vm,
                                         const nu_char_t       *name,
                                         const nu_field_info_t *fields,
                                         nu_u16_t               field_count,
                                         nu_archetype_t        *archetype);

#ifdef NU_IMPL

struct nu__vm
{
    nu__allocator_t        allocator;
    nu__renderer_t         renderer;
    nu__table_manager_t    tables;
    nu__property_manager_t properties;
    nu__resource_manager_t resources;
};

nu_error_t
nu_vm_init (const nu_vm_info_t *info, nu_vm_t *vm)
{
    struct nu__vm     *data;
    nu_vm_properties_t properties;
    nu_error_t         error;

    NU_ASSERT(info->allocator.callback);

    data = info->allocator.callback(sizeof(struct nu__vm),
                                    16,
                                    NU_MEMORY_USAGE_CORE,
                                    info->allocator.userdata);
    NU_CHECK(data, return NU_ERROR_OUT_OF_MEMORY);

    error = nu__allocator_init(&info->allocator, &data->allocator);
    NU_ERROR_CHECK(error, return error);

    {
        nu_cartdata_vm_properties_t data;
        data.props = &properties;
        error      = info->cartridge.load(
            info->cartridge.userdata, NU_CARTDATA_VM_PROPERTIES, &data);
        NU_ERROR_CHECK(error, return error);
    }

    error = nu__table_manager_init(&data->tables,
                                   &data->allocator,
                                   properties.table_capacity,
                                   properties.field_capacity,
                                   properties.chunk_capacity);
    NU_ERROR_CHECK(error, return error);

    error = nu__resource_manager_init(
        &data->allocator, properties.resource_capacity, &data->resources);
    NU_ERROR_CHECK(error, return error);

    /* TODO: load base bundle */

    *vm = data;

    return NU_ERROR_NONE;
}
nu_error_t
nu_vm_free (nu_vm_t vm)
{
    (void)vm;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_tick (nu_vm_t vm)
{
    (void)vm;
    return NU_ERROR_NONE;
}

nu_error_t
nu_vm_create_archetype (nu_vm_t                vm,
                        const nu_char_t       *name,
                        const nu_field_info_t *fields,
                        nu_u16_t               field_count,
                        nu_archetype_t        *archetype)
{
    nu_archetype_t handle;
    nu_error_t     error;
    error
        = nu__archetype_create(&vm->tables, name, fields, field_count, &handle);
    if (archetype)
    {
        *archetype = handle;
    }
    return error;
}

#endif

#endif
