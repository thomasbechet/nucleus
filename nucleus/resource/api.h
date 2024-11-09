#ifndef NU_RESOURCE_API_H
#define NU_RESOURCE_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_HANDLE(nu_resource_t);

typedef void *(*nu_resource_load_t)(nu_seria_t seria);
typedef void (*nu_resource_save_t)(void *data, nu_seria_t seria);
typedef void (*nu_resource_added_t)(void *data);
typedef void (*nu_resource_removed_t)(void *data);

NU_API nu_resource_t nu_resource_register(nu_uid_t              uid,
                                          nu_resource_added_t   added,
                                          nu_resource_removed_t removed,
                                          nu_resource_load_t    load,
                                          nu_resource_save_t    save);
NU_API nu_resource_t nu_resource_find_type(nu_uid_t uid);

NU_API void  nu_resource_add(nu_resource_t type, nu_uid_t uid, void *data);
NU_API void  nu_resource_remove(nu_uid_t uid);
NU_API void  nu_resource_remove_bundle(nu_uid_t uid);
NU_API void  nu_resource_bind_bundle(nu_uid_t uid);
NU_API void *nu_resource_data(nu_resource_t type, nu_uid_t uid);
NU_API void  nu_resource_acquire(nu_uid_t uid);
NU_API void  nu_resource_release(nu_uid_t uid);
NU_API nu_resource_t nu_resource_type(nu_uid_t uid);
NU_API nu_uid_t      nu_resource_load(nu_resource_t type, nu_seria_t seria);
NU_API void          nu_resource_save(nu_uid_t uid, nu_seria_t seria);
NU_API nu_uid_t      nu_resource_load_bundle(nu_seria_t seria);
NU_API void          nu_resource_save_bundle(nu_uid_t uid, nu_seria_t seria);

#endif
