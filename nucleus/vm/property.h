#ifndef NU_PROPERTY_H
#define NU_PROPERTY_H

#include <nucleus/vm/string.h>
#include <nucleus/vm/types.h>

typedef struct
{
    nu_u16_t   table_capacity;
    nu_u16_t   field_capacity;
    nu_u16_t   chunk_capacity;
    nu_u32_t   bytecode_capacity;
    nu_u32_t   resource_capacity;
    nu_ident_t base_bundle;
} nu_vm_properties_t;

NU_API void nu_vm_properties_default(nu_vm_properties_t *props);

#ifdef NU_IMPL

typedef struct
{
    nu_vm_properties_t vm_properties;
} nu__property_manager_t;

void
nu_vm_properties_default (nu_vm_properties_t *props)
{
    props->table_capacity    = 64;
    props->field_capacity    = 512;
    props->chunk_capacity    = 128;
    props->bytecode_capacity = 1024;
    props->resource_capacity = 512;
    nu_ident_set_str(props->base_bundle, "main");
}

#endif

#endif
