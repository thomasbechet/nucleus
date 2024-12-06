#ifndef NU_ECS_API_H
#define NU_ECS_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_OBJECT(nu_ecs_t);

#define NU_ECS_COMPONENT_MAX 32
#define NU_ECS_ATTRIBUTE_MAX 128
#define NU_ECS_ITER_MAX      32

#define NU_ECS_ITER(ecs, iter, e)                           \
    for (nu_ecs_id_t(e) = nu_ecs_begin((ecs), (iter)); (e); \
         (e)            = nu_ecs_next((ecs), (iter)))

#define NU_ECS_COMPONENT(name, struct) \
    nu_ecs_register_component(NU_STR(#name), sizeof(struct))

#define NU_ECS_APRIMITIVE(                                    \
    component, struct, field, field_primitive, field_count)   \
    nu_ecs_register_attribute(                                \
        (component),                                          \
        NU_STR(#field),                                       \
        (nu_ecs_attribute_t) { .kind = NU_ECS_ATTR_PRIMITIVE, \
                               .p    = (field_primitive) },      \
        offsetof(struct, field),                              \
        (field_count));

#define NU_ECS_ACOMPONENT(                                    \
    component, struct, field, field_component, field_count)   \
    nu_ecs_register_attribute(                                \
        (component),                                          \
        NU_STR(#field),                                       \
        (nu_ecs_attribute_t) { .kind = NU_ECS_ATTR_COMPONENT, \
                               .c    = (field_component) },      \
        offsetof(struct, field),                              \
        (field_count));

NU_DEFINE_ID(nu_ecs_id_t);

typedef enum
{
    NU_ECS_BYTE,
    NU_ECS_U32,
    NU_ECS_F32,
    NU_ECS_V3,
    NU_ECS_Q4,
    NU_ECS_M4,
} nu_ecs_primitive_t;

typedef struct
{
    enum
    {
        NU_ECS_ATTR_PRIMITIVE,
        NU_ECS_ATTR_COMPONENT,
        NU_ECS_ATTR_OBJECT,
        NU_ECS_ATTR_STRING,
    } kind;
    union
    {
        nu_ecs_primitive_t p;
        nu_ecs_id_t        c;
        nu_object_type_t   o;
        nu_size_t          s; // string capacity
    };
} nu_ecs_attribute_t;

NU_API nu_ecs_id_t nu_ecs_register_component(nu_str_t name, nu_size_t size);
NU_API nu_ecs_id_t nu_ecs_register_attribute(nu_ecs_id_t        comp,
                                             nu_str_t           name,
                                             nu_ecs_attribute_t attribute,
                                             nu_size_t          offset,
                                             nu_size_t          count);
NU_API nu_ecs_id_t nu_ecs_find_component(nu_str_t name);
NU_API nu_size_t   nu_ecs_component_size(nu_ecs_id_t c);

NU_API nu_ecs_t    nu_ecs_new(nu_size_t capacity);
NU_API nu_ecs_id_t nu_ecs_add(nu_ecs_t ecs);
NU_API void        nu_ecs_add_at(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API void        nu_ecs_remove(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API nu_bool_t   nu_ecs_valid(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API nu_size_t   nu_ecs_count(nu_ecs_t ecs);
NU_API nu_size_t   nu_ecs_capacity(nu_ecs_t ecs);
NU_API void        nu_ecs_clear(nu_ecs_t ecs);

NU_API void     *nu_ecs_set(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void      nu_ecs_unset(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API nu_bool_t nu_ecs_has(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void     *nu_ecs_get(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void     *nu_ecs_get_a(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t a);

NU_API nu_ecs_id_t nu_ecs_iter(nu_size_t include_count,
                               nu_size_t exclude_count);
NU_API void        nu_ecs_includes(nu_ecs_id_t iter, nu_ecs_id_t c);
NU_API void        nu_ecs_excludes(nu_ecs_id_t iter, nu_ecs_id_t c);
NU_API nu_ecs_id_t nu_ecs_begin(nu_ecs_t ecs, nu_ecs_id_t iter);
NU_API nu_ecs_id_t nu_ecs_next(nu_ecs_t ecs, nu_ecs_id_t iter);

NU_API void     nu_ecs_save(nu_ecs_t ecs, nu_seria_t seria);
NU_API nu_ecs_t nu_ecs_load(nu_seria_t seria);
NU_API void     nu_ecs_read_component(nu_ecs_t    ecs,
                                      nu_ecs_id_t e,
                                      nu_ecs_id_t c,
                                      nu_seria_t  seria);
NU_API void     nu_ecs_write_component(nu_ecs_t    ecs,
                                       nu_ecs_id_t e,
                                       nu_ecs_id_t c,
                                       nu_seria_t  seria);

NU_API void nu_ecs_dump_component(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void nu_ecs_dump_entity(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API void nu_ecs_dump(nu_ecs_t ecs);

#endif
