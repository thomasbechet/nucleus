#ifndef NU_RESOURCE_API_H
#define NU_RESOURCE_API_H

#include <nucleus/graphics/api.h>

NU_DEFINE_HANDLE(nu_resource_t);
NU_DEFINE_HANDLE(nu_bundle_t);

#define NU_RESOURCE_NAME_MAX 32

#define NU_RESOURCE_TEXTURE(name)    \
    ((nu_texture_t)nu_resource_data( \
        nu_resource_find(NU_RESOURCE_TEXTURE, name)))
#define NU_RESOURCE_MODEL(name) \
    ((nu_model_t)nu_resource_data(nu_resource_find(NU_RESOURCE_MODEL, name)))

typedef enum
{
    NU_RESOURCE_TEXTURE,
    NU_RESOURCE_MATERIAL,
    NU_RESOURCE_MODEL,
    NU_RESOURCE_INPUT,
    NU_RESOURCE_UNKNOWN,
} nu_resource_type_t;

typedef struct
{
    nu_byte_t          name[NU_RESOURCE_NAME_MAX];
    nu_resource_type_t type;
    nu_bundle_t        bundle;
} nu_resource_info_t;

typedef void (*nu_resource_load_pfn_t)(void *data, nu_seria_t seria);
typedef void (*nu_resource_save_pfn_t)(void *data, nu_seria_t seria);

NU_API nu_resource_type_t nu_resource_register(nu_str_t               name,
                                               nu_seria_layout_t      layout,
                                               nu_resource_load_pfn_t load,
                                               nu_resource_save_pfn_t save);

NU_API nu_resource_t nu_resource_add2(nu_resource_type_t type, void *data);
NU_API nu_resource_t nu_resource_load(nu_resource_type_t type,
                                      nu_seria_t         seria);
NU_API void          nu_resource_save(nu_resource_t resource, nu_seria_t seria);
NU_API void          nu_resource_load_pkg(nu_seria_t seria);
NU_API void          nu_resource_save_pkg(nu_seria_t seria);

NU_API nu_resource_t nu_resource_add(nu_resource_type_t type, nu_str_t name);
NU_API nu_resource_t nu_resource_find(nu_resource_type_t type, nu_str_t name);
NU_API nu_bool_t     nu_resource_exists(nu_resource_type_t type, nu_str_t name);
NU_API void         *nu_resource_data(nu_resource_t handle);
NU_API nu_resource_info_t nu_resource_info(nu_resource_t handle);

#endif
