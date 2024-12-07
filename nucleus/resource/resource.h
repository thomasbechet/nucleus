#ifndef NU_RES_H
#define NU_RES_H

#include <nucleus/resource/api.h>

typedef struct
{
    nu_uid_t         next_uid;
    nu_object_type_t obj_bundle;
} nu__resource_t;

typedef struct
{
    NU_VEC(nu_object_t) objects;
} nu__bundle_t;

static nu_error_t nu__resource_init(void);
static nu_error_t nu__resource_free(void);

#endif
