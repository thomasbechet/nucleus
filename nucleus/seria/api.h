#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#define NU_SERIA_REG_STRUCT(struct, ...)                                 \
    {                                                                    \
        typedef struct sstruct;                                          \
        nu_seria_layout_t l                                              \
            = nu_seria_register_struct(NU_STR(#struct), sizeof(struct)); \
        __VA_ARGS__                                                      \
    }

#define NU_SERIA_FIELD(field, layout, size)           \
    nu_seria_register_struct_field(l,                 \
                                   NU_STR(#field),    \
                                   layout,            \
                                   size,              \
                                   NU_SERIA_REQUIRED, \
                                   offsetof(sstruct, field));

#define NU_SERIA_REG_ENUM(enum, ...)                               \
    {                                                              \
        nu_seria_layout_t layout                                   \
            = nu_seria_register_enum(NU_STR(#enum), sizeof(enum)); \
        __VA_ARGS__                                                \
    }

#define NU_SERIA_VALUE(name, value) \
    nu_seria_register_enum_value(layout, NU_STR(name), value)

#define NU_SERIA_LAYOUT(type) nu_seria_find_layout(NU_STR(#type))

NU_DEFINE_HANDLE(nu_seria_t);
NU_DEFINE_HANDLE(nu_seria_layout_t);

#define NU_SERIA_BYTE NU_HANDLE_MAKE(nu_seria_layout_t, NU_SERIA_PRIMITIVE_BYTE)
#define NU_SERIA_U32  NU_HANDLE_MAKE(nu_seria_layout_t, NU_SERIA_PRIMITIVE_U32)
#define NU_SERIA_F32  NU_HANDLE_MAKE(nu_seria_layout_t, NU_SERIA_PRIMITIVE_F32)
#define NU_SERIA_STR  NU_HANDLE_MAKE(nu_seria_layout_t, NU_SERIA_PRIMITIVE_STR)
#define NU_SERIA_V3   NU_HANDLE_MAKE(nu_seria_layout_t, NU_SERIA_PRIMITIVE_V3)
#define NU_SERIA_Q4   NU_HANDLE_MAKE(nu_seria_layout_t, NU_SERIA_PRIMITIVE_Q4)

#define NU_VEC_READ(v, seria, layout)                         \
    {                                                         \
        nu_u32_t size;                                        \
        nu_seria_read((seria), NU_SERIA_U32, &size);          \
        NU_VEC_RESIZE(v, size);                               \
        nu_seria_read((ser), (layout), (v)->size, (v)->data); \
    }
#define NU_VEC_WRITE(v, seria, layout) \
    nu_seria_write((seria), (layout), (v)->size, (v)->data);

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_mode_t;

typedef enum
{
    NU_SERIA_JSON,
    NU_SERIA_NBIN
} nu_seria_format_t;

typedef enum
{
    NU_SERIA_PRIMITIVE_BYTE = 0,
    NU_SERIA_PRIMITIVE_U32  = 1,
    NU_SERIA_PRIMITIVE_F32  = 2,
    NU_SERIA_PRIMITIVE_STR  = 3,
    NU_SERIA_PRIMITIVE_V3   = 4,
    NU_SERIA_PRIMITIVE_Q4   = 5,
} nu_seria_primitive_t;

typedef enum
{
    NU_SERIA_OPTIONAL,
    NU_SERIA_REQUIRED,
} nu_seria_flag_t;

static nu_str_t nu_seria_primitive_names[]
    = { NU_STR("byte"), NU_STR("u32"), NU_STR("f32"),
        NU_STR("str"),  NU_STR("v3"),  NU_STR("q4") };

NU_API nu_seria_t nu_seria_create(void);
NU_API void       nu_seria_delete(nu_seria_t seria);

NU_API nu_seria_layout_t nu_seria_register_struct(nu_str_t  name,
                                                  nu_size_t size);
NU_API void              nu_seria_register_struct_field(nu_seria_layout_t type,
                                                        nu_str_t          name,
                                                        nu_seria_layout_t fieldtype,
                                                        nu_size_t         size,
                                                        nu_seria_flag_t   flags,
                                                        nu_size_t         offset);
NU_API nu_seria_layout_t nu_seria_register_enum(nu_str_t name, nu_size_t size);
NU_API void              nu_seria_register_enum_value(nu_seria_layout_t type,
                                                      nu_str_t          name,
                                                      nu_u32_t          value);
NU_API nu_seria_layout_t nu_seria_find_layout(nu_str_t name);
NU_API nu_str_t          nu_seria_name(nu_seria_layout_t layout);
NU_API nu_size_t         nu_seria_size(nu_seria_layout_t layout);

NU_API void nu_seria_dump_layouts(void);
NU_API void nu_seria_dump_values(nu_seria_layout_t layout,
                                 nu_size_t         size,
                                 void             *data);

NU_API void      nu_seria_open_file(nu_seria_t        seria,
                                    nu_seria_mode_t   mode,
                                    nu_seria_format_t format,
                                    nu_str_t          filename);
NU_API void      nu_seria_open_bytes(nu_seria_t        seria,
                                     nu_seria_mode_t   mode,
                                     nu_seria_format_t format,
                                     nu_byte_t        *bytes,
                                     nu_size_t         size);
NU_API nu_size_t nu_seria_close(nu_seria_t seria);

NU_API void nu_seria_write(nu_seria_t        seria,
                           nu_seria_layout_t layout,
                           nu_size_t         size,
                           const void       *data);
NU_API void nu_seria_read(nu_seria_t        seria,
                          nu_seria_layout_t layout,
                          nu_size_t         size,
                          void             *data);

#endif
