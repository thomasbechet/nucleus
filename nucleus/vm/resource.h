#ifndef NU_RESOURCE_H
#define NU_RESOURCE_H

#include <nucleus/vm/allocator.h>
#include <nucleus/vm/error.h>
#include <nucleus/vm/string.h>
#include <nucleus/vm/types.h>

#define NU_FOREACH_RESOURCE(RESOURCE) \
    RESOURCE(BUNDLE)                  \
    RESOURCE(SYSTEM)                  \
    RESOURCE(TEXTURE)
#define NU_GENERATE_RESOURCE_ENUM(RESOURCE) NU_RESOURCE_##RESOURCE,
#define NU_GENERATE_RESOURCE_NAME(RESOURCE) #RESOURCE,
typedef enum
{
    NU_FOREACH_RESOURCE(NU_GENERATE_RESOURCE_ENUM) NU_RESOURCE_UNKNOWN
} nu_resource_type_t;
const nu_char_t *NU_RESOURCE_NAMES[]
    = { NU_FOREACH_RESOURCE(NU_GENERATE_RESOURCE_NAME) "UNKNOWN" };

#ifdef NU_IMPL

typedef nu_u32_t nu__resource_id_t;

typedef struct
{
    nu__resource_id_t first_resource;
} nu__bundle_value_t;

typedef union
{
    nu__bundle_value_t bundle;
} nu__resource_value_t;

typedef struct
{
    nu_ident_t           name;
    nu_resource_type_t   type;
    nu__resource_value_t value;
    nu__resource_id_t    next_in_bundle;
} nu__resource_entry_t;

typedef struct
{
    nu__resource_id_t first_resource;
} nu__bundle_entry_t;

typedef struct
{
    nu_u32_t              capacity;
    nu_u32_t              count;
    nu__resource_entry_t *resources;
} nu__resource_manager_t;

static nu_error_t
nu__resource_manager_init (nu__allocator_t        *allocator,
                           nu_u32_t                resource_capacity,
                           nu__resource_manager_t *manager)
{
    manager->capacity = resource_capacity;
    manager->count    = 0;
    manager->resources
        = nu__alloc(allocator,
                    sizeof(nu__resource_entry_t) * resource_capacity,
                    NU_MEMORY_USAGE_CORE);
    NU_CHECK(manager->resources, return NU_ERROR_OUT_OF_MEMORY);
    return NU_ERROR_NONE;
}

#endif

#endif
