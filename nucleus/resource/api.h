#ifndef NU_RESOURCE_API_H
#define NU_RESOURCE_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_HANDLE(nu_resource_type_t);
NU_DEFINE_HANDLE(nu_resource_t);

typedef void (*nu_resource_load_pfn_t)(void *data, nu_seria_t seria);
typedef void (*nu_resource_save_pfn_t)(void *data, nu_seria_t seria);
typedef void (*nu_resource_free_pfn_t)(void *data);

NU_API nu_resource_type_t nu_resource_register(nu_uid_t               uid,
                                               nu_resource_load_pfn_t load,
                                               nu_resource_save_pfn_t save,
                                               nu_resource_free_pfn_t free);
NU_API nu_resource_type_t nu_resource_type_find(nu_uid_t uid);

NU_API nu_resource_t nu_resource_add(nu_resource_type_t type,
                                     nu_uid_t           uid,
                                     void              *data);
NU_API void          nu_resource_remove(nu_resource_t resource);
NU_API void         *nu_resource_data(nu_resource_t resource);
NU_API nu_resource_t nu_resource_find(nu_uid_t uid);
NU_API void          nu_resource_acquire(nu_resource_t resource);
NU_API void          nu_resource_release(nu_resource_t resource);

NU_API nu_resource_type_t nu_resource_get_type(nu_resource_t resource);
NU_API nu_uid_t           nu_resource_get_uid(nu_resource_t resource);

NU_API nu_resource_t nu_resource_load(nu_resource_type_t type,
                                      nu_seria_t         seria);
NU_API void          nu_resource_save(nu_resource_t resource, nu_seria_t seria);
NU_API void          nu_resource_load_pkg(nu_seria_t seria);
NU_API void          nu_resource_save_pkg(nu_seria_t seria);

#endif
