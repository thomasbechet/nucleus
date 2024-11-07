#ifndef NU_RESOURCE_H
#define NU_RESOURCE_H

#include <nucleus/resource/api.h>

typedef struct
{
    nu_bool_t          used;
    nu_resource_type_t type;
    nu_u32_t           hash;
    void              *data;
    nu_byte_t          name[NU_RESOURCE_NAME_MAX];
    nu_size_t          name_size;
    nu_bundle_t        bundle;
    nu_size_t          refcount;
} nu__resource_entry_t;

typedef struct
{
    NU_POOL(nu__resource_entry_t) entries;
    nu_bundle_t active_bundle;
} nu__resource_t;

static nu_error_t nu__resource_init(void);
static nu_error_t nu__resource_free(void);

#endif
