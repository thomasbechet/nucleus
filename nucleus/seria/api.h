#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

NU_DEFINE_OBJECT(nu_seria_t);

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_mode_t;

typedef void (*nu_object_seria_load_t)(nu_seria_t seria, void *data);
typedef void (*nu_object_seria_save_t)(nu_seria_t seria, const void *data);

NU_API nu_seria_t nu_seria_new_file(nu_str_t        filename,
                                    nu_seria_mode_t mode,
                                    nu_size_t       buffer_size);
NU_API nu_seria_t nu_seria_new_bytes(nu_seria_mode_t mode,
                                     nu_byte_t      *bytes,
                                     nu_size_t       size);

NU_API nu_size_t nu_byte_load(nu_seria_t seria, nu_size_t count, nu_byte_t *p);
NU_API void nu_byte_save(nu_seria_t seria, nu_size_t count, const nu_byte_t *p);
NU_API nu_size_t nu_u32_load(nu_seria_t seria, nu_size_t count, nu_u32_t *p);
NU_API void nu_u32_save(nu_seria_t seria, nu_size_t count, const nu_u32_t *p);
NU_API nu_size_t nu_f32_load(nu_seria_t seria, nu_size_t count, nu_f32_t *p);
NU_API void nu_f32_save(nu_seria_t seria, nu_size_t count, const nu_f32_t *p);

NU_API nu_str_t nu_str_load(nu_seria_t       seria,
                            nu_size_t        capacity,
                            const nu_byte_t *buffer);
NU_API void     nu_str_save(nu_seria_t seria, nu_str_t str);

NU_API void        nu_object_set_seria(nu_object_type_t       type,
                                       nu_object_seria_load_t load,
                                       nu_object_seria_save_t save);
NU_API nu_object_t nu_object_load(nu_seria_t seria, nu_object_type_t type);
NU_API void        nu_object_save(nu_seria_t seria, nu_object_t object);

#endif
