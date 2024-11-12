#ifndef NU_RESOURCE_H
#define NU_RESOURCE_H

#include <nucleus/resource/api.h>

typedef struct
{
    nu_uid_t             uid;
    nu_resource_load_t   load;
    nu_resource_unload_t unload;
    nu_resource_save_t   save;
} nu__resource_type_t;

typedef struct
{
    nu_uid_t  type;
    nu_uid_t  uid;
    nu_uid_t  bundle;
    nu_size_t refcount;
    void     *data;
} nu__resource_entry_t;

typedef struct
{
    NU_VEC(nu__resource_type_t) types;
    NU_VEC(nu__resource_entry_t) entries;
    nu_uid_t active_bundle;
} nu__resource_t;

static nu_error_t nu__resource_init(void);
static nu_error_t nu__resource_free(void);

#endif
