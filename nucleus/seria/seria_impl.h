#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>

// #include <nucleus/seria/reflect_impl.h>
// #ifdef NU_BUILD_JSMN
// #include <nucleus/seria/json_impl.h>
// #endif
//

#define NU__REGISTER_CORE(enum, coretype)                        \
    {                                                            \
        nu__seria_type_t *type = NU_VEC_PUSH(&_ctx.seria.types); \
        type->kind             = NU__SERIA_PRIMITIVE;            \
        type->name             = nu_seria_primitive_names[enum]; \
        type->size             = sizeof(coretype);               \
        type->primitive        = enum;                           \
    }

static void
nu__seria_register_primitive_types (void)
{
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_U32, nu_u32_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_F32, nu_f32_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_V3, nu_v3_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_Q4, nu_q4_t);
}

#undef NU__REGISTER_CORE

static void
nu__seria_init (void)
{
    NU_POOL_INIT(1, &_ctx.seria.instances);
    NU_VEC_INIT(10, &_ctx.seria.types);
    nu__seria_register_primitive_types();
}
static void
nu__seria_free (void)
{
    for (nu_size_t i = 0; i < _ctx.seria.types.size; ++i)
    {
        nu__seria_type_t *p = _ctx.seria.types.data + i;
        switch (p->kind)
        {
            case NU__SERIA_PRIMITIVE:
                NU_VEC_FREE(&_ctx.seria.types.data[i].values);
                break;
            case NU__SERIA_STRUCT:
                NU_VEC_FREE(&_ctx.seria.types.data[i].fields);
                break;
            case NU__SERIA_ENUM:
                break;
        }
    }
    NU_VEC_FREE(&_ctx.seria.types);
    NU_POOL_FREE(&_ctx.seria.instances);
}

nu_seria_t
nu_seria_create (void)
{
    nu_size_t             index;
    nu__seria_instance_t *s = NU_POOL_ADD(&_ctx.seria.instances, &index);
    s->is_open              = NU_FALSE;
    return NU_HANDLE_MAKE(nu_seria_t, index);
}
void
nu_seria_delete (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->is_open)
    {
        nu_seria_close(seria);
    }
    NU_POOL_REMOVE(&_ctx.seria.instances, NU_HANDLE_INDEX(seria));
}

void
nu_seria_open_reflect (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    s->is_open = NU_TRUE;
}
void
nu_seria_open_dumper (nu_seria_t seria)
{
}
#ifdef NU_BUILD_JSMN
void
nu_seria_open_json (nu_seria_t       seria,
                    const jsmntok_t *toks,
                    nu_size_t        toks_count)
{
}
#endif
void
nu_seria_close (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
}

nu_seria_type_t
nu_seria_register_struct (const nu_char_t *name, nu_size_t size)
{
    nu__seria_type_t *type = NU_VEC_PUSH(&_ctx.seria.types);
    type->kind             = NU__SERIA_STRUCT;
    type->name             = name;
    type->size             = size;
    NU_VEC_INIT(5, &type->fields);
    return NU_HANDLE_MAKE(nu_seria_type_t, _ctx.seria.types.size - 1);
}
void
nu_seria_register_struct_field (nu_seria_type_t  type,
                                const nu_char_t *name,
                                nu_seria_type_t  fieldtype,
                                nu_size_t        size,
                                nu_seria_flag_t  flags,
                                nu_size_t        offset)
{
    NU_ASSERT(fieldtype);
    NU_ASSERT(type && name && size);
    nu__seria_type_t         *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    nu__seria_struct_field_t *a = NU_VEC_PUSH(&p->fields);
    NU_ASSERT(p->kind == NU__SERIA_STRUCT);
    a->name   = name;
    a->size   = size;
    a->type   = fieldtype;
    a->offset = offset;
    a->flags  = flags;
}
nu_seria_type_t
nu_seria_register_enum (const nu_char_t *name, nu_size_t size)
{
    nu__seria_type_t *type = NU_VEC_PUSH(&_ctx.seria.types);
    type->kind             = NU__SERIA_ENUM;
    type->name             = name;
    type->size             = size;
    NU_VEC_INIT(5, &type->values);
    return NU_HANDLE_MAKE(nu_seria_type_t, _ctx.seria.types.size - 1);
}
void
nu_seria_register_enum_value (nu_seria_type_t  type,
                              const nu_char_t *name,
                              nu_u32_t         value)
{
    nu__seria_type_t       *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    nu__seria_enum_value_t *v = NU_VEC_PUSH(&p->values);
    NU_ASSERT(p->kind == NU__SERIA_ENUM);
    v->name  = name;
    v->value = value;
}
nu_seria_type_t
nu_seria_type (const nu_char_t *name)
{
    for (nu_size_t i = 0; i < _ctx.seria.types.size; ++i)
    {
        if (NU_MATCH(name, _ctx.seria.types.data[i].name))
        {
            return NU_HANDLE_MAKE(nu_seria_type_t, i);
        }
    }
    return NU_NULL;
}
void

nu_seria_dump_types (void)
{
    for (nu_size_t i = 0; i < _ctx.seria.types.size; ++i)
    {
        nu__seria_type_t *p = _ctx.seria.types.data + i;
        switch (p->kind)
        {
            case NU__SERIA_PRIMITIVE:
                // don't print core types
                break;
            case NU__SERIA_STRUCT:
                NU_INFO("%s <%d> {", p->name, p->size);
                for (nu_size_t f = 0; f < p->fields.size; ++f)
                {
                    nu__seria_struct_field_t *pf = p->fields.data + f;
                    nu__seria_type_t         *subtype
                        = _ctx.seria.types.data + NU_HANDLE_INDEX(pf->type);
                    const nu_char_t *flags;
                    if (pf->flags == NU_SERIA_REQUIRED)
                    {
                        flags = "REQ";
                    }
                    else
                    {
                        flags = "OPT";
                    }
                    NU_INFO("   %s %s [%d] %s = <%d,%d>",
                            pf->name,
                            subtype->name,
                            pf->size,
                            flags,
                            pf->offset,
                            subtype->size);
                }
                NU_INFO("}");
                break;
            case NU__SERIA_ENUM:
                NU_INFO("%s <%d> {", p->name, p->size);
                for (nu_size_t v = 0; v < p->values.size; ++v)
                {
                    nu__seria_enum_value_t *pv = p->values.data + v;
                    NU_INFO("   %s = %d", pv->name, pv->value);
                }
                NU_INFO("}");
                break;
        }
    }
}
static void
nu__seria_print_with_depth (nu_size_t depth, const nu_char_t *format, ...)
{
    va_list args;
    va_start(args, format);

    nu_char_t str[256];
    nu_vsnprintf(str, 256, format, args);
    NU_INFO("%*s%s", depth * 2, "", str);

    va_end(args);
}
static void
nu__seria_dump (nu_size_t       depth,
                nu_seria_type_t type,
                nu_size_t       size,
                nu_byte_t      *data)
{
    NU_ASSERT(type && data);
    nu__seria_type_t *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    for (nu_size_t i = 0; i < size; ++i)
    {
        nu_byte_t *ptr = (nu_byte_t *)((nu_size_t)data + i * p->size);
        switch (p->kind)
        {
            case NU__SERIA_PRIMITIVE:
                switch (p->primitive)
                {
                    case NU_SERIA_PRIMITIVE_U32:
                        nu__seria_print_with_depth(
                            depth, "%d", *(nu_u32_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_F32:
                        nu__seria_print_with_depth(
                            depth, "%lf", *(nu_f32_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_V3: {
                        nu_v3_t *v = (nu_v3_t *)ptr;
                        nu__seria_print_with_depth(
                            depth, "[" NU_V3_FORMAT "]", v->x, v->y, v->z);
                    }
                    break;
                    case NU_SERIA_PRIMITIVE_Q4: {
                        nu_q4_t *v = (nu_q4_t *)ptr;
                        nu__seria_print_with_depth(depth,
                                                   "[ %lf %lf %lf %lf ]",
                                                   v->x,
                                                   v->y,
                                                   v->z,
                                                   v->w);
                    }
                    break;
                }
                break;
            case NU__SERIA_STRUCT: {
                nu__seria_print_with_depth(depth, "{");
                for (nu_size_t f = 0; f < p->fields.size; ++f)
                {
                    nu__seria_struct_field_t *field = p->fields.data + f;
                    nu__seria_type_t         *subtype
                        = _ctx.seria.types.data + NU_HANDLE_INDEX(field->type);
                    nu__seria_print_with_depth(
                        depth + 1, "%s (%s):", field->name, subtype->name);
                    nu__seria_dump(depth + 2,
                                   field->type,
                                   field->size,
                                   ptr + field->offset);
                }
                nu__seria_print_with_depth(depth, "}");
            }
            break;
            case NU__SERIA_ENUM: {
                nu_u32_t  value = *(nu_u32_t *)ptr;
                nu_bool_t found = NU_FALSE;
                for (nu_size_t e = 0; e < p->values.size; ++e)
                {
                    if (value == p->values.data[e].value)
                    {
                        nu__seria_print_with_depth(
                            depth, "%s", p->values.data[e].name);
                        found = NU_TRUE;
                        break;
                    }
                }
                if (!found)
                {
                    nu__seria_print_with_depth(depth, "#INVALID");
                }
            }
            break;
        }
    }
}
void
nu_seria_dump (nu_seria_type_t type, nu_size_t size, void *data)
{
    nu__seria_dump(0, type, size, data);
}

#endif
