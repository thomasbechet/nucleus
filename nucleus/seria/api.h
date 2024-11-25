#ifndef NU_SERIA_API_H
#define NU_SERIA_API_H

#include <nucleus/core/api.h>

#define NU_SERIA_STRUCT(struct, ...)                                     \
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
#define NU_SERIA_OBJREF(field, type, size)             \
    nu_seria_register_struct_objref(l,                 \
                                    NU_STR(#field),    \
                                    type,              \
                                    size,              \
                                    NU_SERIA_REQUIRED, \
                                    offsetof(sstruct, field));

#define NU_SERIA_ENUM(enum, ...)                                   \
    {                                                              \
        nu_seria_layout_t layout                                   \
            = nu_seria_register_enum(NU_STR(#enum), sizeof(enum)); \
        __VA_ARGS__                                                \
    }

#define NU_SERIA_VALUE(name, value) \
    nu_seria_register_enum_value(layout, NU_STR(name), value)

#define NU_SERIA_LAYOUT(type) nu_seria_find_layout(NU_STR(#type))

NU_DEFINE_OBJECT(nu_seria_t);
NU_DEFINE_OBJECT(nu_seria_layout_t);

#define NU_SERIA_LAYOUT_MAX       64
#define NU_SERIA_STRUCT_FIELD_MAX 128
#define NU_SERIA_ENUM_VALUE_MAX   64

#define NU_FIXEDVEC_READ(v, seria, layout)                    \
    {                                                         \
        nu_u32_t size = nu_seria_read_u32((seria));           \
        NU_FIXEDVEC_RESIZE(v, size);                          \
        nu_seria_read((ser), (layout), (v)->size, (v)->data); \
    }
#define NU_FIXEDVEC_WRITE(v, seria, layout)                      \
    {                                                            \
        nu_seria_write_u32((v)->size);                           \
        nu_seria_write((seria), (layout), (v)->size, (v)->data); \
    }

typedef enum
{
    NU_SERIA_READ,
    NU_SERIA_WRITE
} nu_seria_mode_t;

typedef enum
{
    NU_SERIA_PRIMITIVE_BYTE  = 0,
    NU_SERIA_PRIMITIVE_U32   = 1,
    NU_SERIA_PRIMITIVE_F32   = 2,
    NU_SERIA_PRIMITIVE_STR   = 3,
    NU_SERIA_PRIMITIVE_V3    = 4,
    NU_SERIA_PRIMITIVE_Q4    = 5,
    NU_SERIA_PRIMITIVE_COUNT = 6
} nu_seria_primitive_t;

typedef enum
{
    NU_SERIA_OPTIONAL,
    NU_SERIA_REQUIRED,
} nu_seria_flag_t;

static nu_str_t nu_seria_primitive_names[]
    = { NU_STR("byte"), NU_STR("u32"), NU_STR("f32"),
        NU_STR("str"),  NU_STR("v3"),  NU_STR("q4") };

NU_API nu_seria_t nu_seria_new(void);

NU_API nu_seria_layout_t nu_seria_register_struct(nu_str_t  name,
                                                  nu_size_t size);
NU_API void nu_seria_register_struct_field(nu_seria_layout_t layout,
                                           nu_str_t          name,
                                           nu_seria_layout_t fieldlayout,
                                           nu_size_t         size,
                                           nu_seria_flag_t   flags,
                                           nu_size_t         offset);
NU_API void nu_seria_register_struct_objref(nu_seria_layout_t layout,
                                            nu_str_t          name,
                                            nu_object_type_t  type,
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

NU_API void      nu_seria_open_file(nu_seria_t      seria,
                                    nu_seria_mode_t mode,
                                    nu_str_t        filename,
                                    nu_size_t       buffer_size);
NU_API void      nu_seria_open_bytes(nu_seria_t      seria,
                                     nu_seria_mode_t mode,
                                     nu_byte_t      *bytes,
                                     nu_size_t       size);
NU_API nu_size_t nu_seria_close(nu_seria_t seria);

NU_API void nu_seria_write(nu_seria_t        seria,
                           nu_seria_layout_t layout,
                           nu_size_t         size,
                           const void       *data);
NU_API void nu_seria_read(nu_seria_t        seria,
                          nu_seria_layout_t layout,
                          nu_size_t         size,
                          void             *data);

NU_API nu_seria_layout_t nu_seria_primitive(nu_seria_primitive_t primitive);

#define NU_SERIA_BYTE nu_seria_primitive(NU_SERIA_PRIMITIVE_BYTE)
#define NU_SERIA_U32  nu_seria_primitive(NU_SERIA_PRIMITIVE_U32)
#define NU_SERIA_F32  nu_seria_primitive(NU_SERIA_PRIMITIVE_F32)
#define NU_SERIA_STR  nu_seria_primitive(NU_SERIA_PRIMITIVE_STR)
#define NU_SERIA_V3   nu_seria_primitive(NU_SERIA_PRIMITIVE_V3)
#define NU_SERIA_Q4   nu_seria_primitive(NU_SERIA_PRIMITIVE_Q4)

#define NU_DEFINE_SERIA_PRIMITIVE(ident, type)               \
    NU_API void nu_seria_read_##ident(                       \
        nu_seria_t seria, nu_size_t size, type *data);       \
    NU_API void nu_seria_write_##ident(                      \
        nu_seria_t seria, nu_size_t size, const type *data); \
    NU_API type nu_seria_read_1##ident(nu_seria_t seria);    \
    NU_API void nu_seria_write_1##ident(nu_seria_t seria, type v);

NU_DEFINE_SERIA_PRIMITIVE(byte, nu_byte_t);
NU_DEFINE_SERIA_PRIMITIVE(u32, nu_u32_t);
NU_DEFINE_SERIA_PRIMITIVE(f32, nu_f32_t);
NU_DEFINE_SERIA_PRIMITIVE(v3, nu_v3_t);
NU_DEFINE_SERIA_PRIMITIVE(q4, nu_q4_t);

NU_API void        nu_seria_write_str(nu_seria_t seria, nu_str_t str);
NU_API nu_str_t    nu_seria_read_str(nu_seria_t seria,
                                     nu_size_t  capacity,
                                     nu_byte_t *buffer);
NU_API void        nu_seria_write_objref(nu_seria_t seria, nu_object_t objref);
NU_API nu_object_t nu_seria_read_objref(nu_seria_t       seria,
                                        nu_object_type_t type);

#endif
