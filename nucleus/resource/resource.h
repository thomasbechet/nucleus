#ifndef NU_RESOURCE_H
#define NU_RESOURCE_H

#include <nucleus/resource/api.h>

#define NU__RESOURCE_TYPE_NAME_MAX 32
#define NU__RESOURCE_NAME_MAX      32

typedef struct
{
    nu_uid_t               uid;
    nu_resource_load_pfn_t load;
    nu_resource_save_pfn_t save;
    nu_resource_free_pfn_t free;
} nu__resource_type_t;

typedef struct
{
    nu_bool_t          used;
    nu_uid_t           uid;
    void              *data;
    nu_size_t          refcount;
    nu_resource_type_t type;
} nu__resource_entry_t;

typedef struct
{
    NU_VEC(nu__resource_type_t) types;
    NU_POOL(nu__resource_entry_t) entries;
} nu__resource_t;

static nu_error_t nu__resource_init(void);
static nu_error_t nu__resource_free(void);

#endif
