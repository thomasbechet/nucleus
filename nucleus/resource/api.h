#ifndef NU_RESOURCE_API_H
#define NU_RESOURCE_API_H

#include <nucleus/seria/api.h>

typedef void *(*nu_resource_load_t)(nu_seria_t seria);
typedef void (*nu_resource_unload_t)(void *data);
typedef void (*nu_resource_save_t)(void *data, nu_seria_t seria);

NU_API void nu_resource_register(nu_uid_t             uid,
                                 nu_resource_load_t   load,
                                 nu_resource_unload_t unload,
                                 nu_resource_save_t   save);

NU_API void     nu_resource_set_default_bundle(nu_uid_t uid);
NU_API void     nu_resource_set_bundle(nu_uid_t uid, nu_uid_t bundle);
NU_API void     nu_resource_acquire(nu_uid_t uid);
NU_API void     nu_resource_release(nu_uid_t uid);
NU_API void    *nu_resource_data(nu_uid_t type, nu_uid_t uid);
NU_API nu_uid_t nu_resource_type(nu_uid_t uid);

// insert / load / unload / save
NU_API void     nu_resource_insert(nu_uid_t type, nu_uid_t uid, void *data);
NU_API nu_uid_t nu_resource_load(nu_seria_t seria);
NU_API nu_uid_t nu_resource_load_bundle(nu_seria_t seria);
NU_API void     nu_resource_unload(nu_uid_t uid);
NU_API void     nu_resource_unload_bundle(nu_uid_t uid);
NU_API void     nu_resource_save(nu_uid_t uid, nu_seria_t seria);
NU_API void     nu_resource_save_bundle(nu_uid_t uid, nu_seria_t seria);

#endif
