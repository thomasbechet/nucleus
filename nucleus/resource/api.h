#ifndef NU_RES_API_H
#define NU_RES_API_H

#include <nucleus/seria/api.h>

NU_API nu_uid_t nu_resource_load(nu_seria_t seria);
NU_API void     nu_resource_save(nu_uid_t group, nu_seria_t seria);

#endif
