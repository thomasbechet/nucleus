#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

NU_DEFINE_OBJECT(nu_seria_t);

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_mode_t;

typedef nu_object_t (*nu_seria_load_object_t)(nu_seria_t seria);
typedef void (*nu_seria_save_object_t)(nu_seria_t seria, nu_object_t object);

NU_API nu_seria_t nu_seria_new_file(nu_str_t        filename,
                                    nu_seria_mode_t mode,
                                    nu_size_t       buffer_size);
NU_API nu_seria_t nu_seria_new_bytes(nu_seria_mode_t mode,
                                     nu_byte_t      *bytes,
                                     nu_size_t       size);
NU_API void       nu_seria_flush(nu_seria_t seria);

NU_API void nu_seria_read_byte(nu_seria_t seria, nu_size_t count, nu_byte_t *p);
NU_API void nu_seria_write_byte(nu_seria_t       seria,
                                nu_size_t        count,
                                const nu_byte_t *p);
NU_API void nu_seria_read_u32(nu_seria_t seria, nu_size_t count, nu_u32_t *p);
NU_API void nu_seria_write_u32(nu_seria_t      seria,
                               nu_size_t       count,
                               const nu_u32_t *p);
NU_API void nu_seria_read_f32(nu_seria_t seria, nu_size_t count, nu_f32_t *p);
NU_API void nu_seria_write_f32(nu_seria_t      seria,
                               nu_size_t       count,
                               const nu_f32_t *p);
NU_API void nu_seria_read_v2(nu_seria_t seria, nu_size_t count, nu_v2_t *p);
NU_API void nu_seria_write_v2(nu_seria_t     seria,
                              nu_size_t      count,
                              const nu_v2_t *p);
NU_API void nu_seria_read_v3(nu_seria_t seria, nu_size_t count, nu_v3_t *p);
NU_API void nu_seria_write_v3(nu_seria_t     seria,
                              nu_size_t      count,
                              const nu_v3_t *p);
NU_API void nu_seria_read_q4(nu_seria_t seria, nu_size_t count, nu_q4_t *p);
NU_API void nu_seria_write_q4(nu_seria_t     seria,
                              nu_size_t      count,
                              const nu_q4_t *p);
NU_API void nu_seria_read_m4(nu_seria_t seria, nu_size_t count, nu_m4_t *p);
NU_API void nu_seria_write_m4(nu_seria_t     seria,
                              nu_size_t      count,
                              const nu_m4_t *p);
NU_API nu_object_t nu_seria_read_object(nu_seria_t       seria,
                                        nu_object_type_t type);
NU_API void        nu_seria_write_object(nu_seria_t seria, nu_object_t obj);
NU_API nu_str_t    nu_seria_read_str(nu_seria_t seria,
                                     nu_size_t  capacity,
                                     nu_byte_t *buffer);
NU_API void        nu_seria_write_str(nu_seria_t seria, nu_str_t str);

NU_API void        nu_object_set_seria(nu_object_type_t       type,
                                       nu_seria_load_object_t load,
                                       nu_seria_save_object_t save);
NU_API nu_object_t nu_seria_load_object(nu_seria_t       seria,
                                        nu_object_type_t type);
NU_API void        nu_seria_save_object(nu_seria_t seria, nu_object_t object);

#endif
