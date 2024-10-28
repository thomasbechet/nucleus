#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#define NU_SERIA_STRUCT(name, struct, ...)                            \
    {                                                                 \
        typedef struct stype;                                         \
        nu_seria_type_t t                                             \
            = nu_seria_register_struct(NU_STR(name), sizeof(struct)); \
        __VA_ARGS__                                                   \
    }

#define NU_SERIA_STRUCT_FIELD(name, type, size, flags, field) \
    nu_seria_register_struct_field(                           \
        t, NU_STR(name), type, size, flags, offsetof(stype, field));

#define NU_SERIA_ENUM(name, enum, ...)                            \
    {                                                             \
        nu_seria_type_t t                                         \
            = nu_seria_register_enum(NU_STR(name), sizeof(enum)); \
        __VA_ARGS__                                               \
    }

#define NU_SERIA_ENUM_VALUE(name, value) \
    nu_seria_register_enum_value(t, NU_STR(name), value)

NU_DEFINE_HANDLE(nu_seria_t);
NU_DEFINE_HANDLE(nu_seria_type_t);
NU_DEFINE_HANDLE(nu_seria_buffer_t);

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_io_t;

typedef enum
{
    NU_SERIA_JSON,
    NU_SERIA_NBIN
} nu_seria_format_t;

typedef enum
{
    NU_SERIA_PRIMITIVE_BUF = 0,
    NU_SERIA_PRIMITIVE_U32 = 1,
    NU_SERIA_PRIMITIVE_F32 = 2,
    NU_SERIA_PRIMITIVE_V3  = 3,
    NU_SERIA_PRIMITIVE_Q4  = 4,
} nu_seria_primitive_t;

typedef enum
{
    NU_SERIA_OPTIONAL,
    NU_SERIA_REQUIRED,
} nu_seria_flag_t;

static nu_str_t nu_seria_primitive_names[] = {
    NU_STR("buf"), NU_STR("u32"), NU_STR("f32"), NU_STR("v3"), NU_STR("q4")
};

#define NU_SERIA_BUF NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_BUF)
#define NU_SERIA_U32 NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_U32)
#define NU_SERIA_F32 NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_F32)
#define NU_SERIA_V3  NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_V3)
#define NU_SERIA_Q4  NU_HANDLE_MAKE(nu_seria_type_t, NU_SERIA_PRIMITIVE_Q4)

NU_API nu_seria_t nu_seria_create(void);
NU_API void       nu_seria_delete(nu_seria_t seria);

NU_API nu_seria_type_t nu_seria_register_struct(nu_str_t name, nu_size_t size);
NU_API void            nu_seria_register_struct_field(nu_seria_type_t type,
                                                      nu_str_t        name,
                                                      nu_seria_type_t fieldtype,
                                                      nu_size_t       count,
                                                      nu_seria_flag_t flags,
                                                      nu_size_t       offset);
NU_API nu_seria_type_t nu_seria_register_enum(nu_str_t name, nu_size_t size);
NU_API void            nu_seria_register_enum_value(nu_seria_type_t type,
                                                    nu_str_t        name,
                                                    nu_u32_t        value);
NU_API nu_seria_type_t nu_seria_type(nu_str_t name);

NU_API void nu_seria_dump_types(void);
NU_API void nu_seria_dump_values(nu_seria_type_t type,
                                 nu_size_t       count,
                                 void           *data);

NU_API void nu_seria_open_file(nu_seria_t        seria,
                               nu_seria_io_t     mode,
                               nu_seria_format_t format,
                               nu_str_t          filename);
NU_API void nu_seria_open_bytes(nu_seria_t        seria,
                                nu_seria_io_t     mode,
                                nu_seria_format_t format,
                                const nu_byte_t  *bytes,
                                nu_size_t         size);
NU_API void nu_seria_close(nu_seria_t seria);

NU_API void      nu_seria_seek(nu_seria_t seria, nu_seria_buffer_t buffer);
NU_API nu_size_t nu_seria_read(nu_seria_t      seria,
                               nu_seria_type_t type,
                               nu_size_t       count,
                               void           *data);
NU_API void nu_seria_write_root(nu_seria_t seria, nu_seria_buffer_t buffer);
NU_API nu_seria_buffer_t nu_seria_write(nu_seria_t      seria,
                                        nu_seria_type_t type,
                                        nu_size_t       count,
                                        void           *data);

#endif
