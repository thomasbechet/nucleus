#ifndef NU_RES_API_H
#define NU_RES_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_OBJECT(nu_bundle_t);

NU_API nu_bundle_t nu_bundle_new(nu_size_t capacity);
NU_API void        nu_bundle_add(nu_bundle_t bundle, nu_object_t object);
NU_API nu_size_t   nu_bundle_size(nu_bundle_t bundle);
NU_API nu_object_t nu_bundle_get(nu_bundle_t bundle, nu_size_t index);
NU_API nu_bundle_t nu_bundle_load(nu_seria_t seria);
NU_API void        nu_bundle_save(nu_bundle_t bundle, nu_seria_t seria);

#endif
