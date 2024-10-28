#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/seria/nbin_impl.h>
#ifdef NU_BUILD_SERIA_JSON
#include <nucleus/seria/json_impl.h>
#endif

static nu_byte_t *
nu__seria_bytes_load_file (nu_str_t filename, nu_size_t *size)
{
    char fn[256];
    nu_str_to_cstr(filename, fn, 256);
    FILE *f = fopen(fn, "rb");
    if (!f)
    {
        NU_ERROR("failed to open file " NU_STR_FMT, NU_STR_ARGS(filename));
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)nu_alloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}

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
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_BUF, nu_seria_buffer_t);
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
    s->opened               = NU_FALSE;
    s->bytes                = NU_NULL;
    return NU_HANDLE_MAKE(nu_seria_t, index);
}
void
nu_seria_delete (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->opened)
    {
        nu_seria_close(seria);
    }
    NU_POOL_REMOVE(&_ctx.seria.instances, NU_HANDLE_INDEX(seria));
}

nu_seria_type_t
nu_seria_register_struct (nu_str_t name, nu_size_t size)
{
    nu__seria_type_t *type = NU_VEC_PUSH(&_ctx.seria.types);
    type->kind             = NU__SERIA_STRUCT;
    type->name             = name;
    type->size             = size;
    NU_VEC_INIT(5, &type->fields);
    return NU_HANDLE_MAKE(nu_seria_type_t, _ctx.seria.types.size - 1);
}
void
nu_seria_register_struct_field (nu_seria_type_t type,
                                nu_str_t        name,
                                nu_seria_type_t fieldtype,
                                nu_size_t       count,
                                nu_seria_flag_t flags,
                                nu_size_t       offset)
{
    NU_ASSERT(fieldtype);
    NU_ASSERT(type && count);
    nu__seria_type_t         *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    nu__seria_struct_field_t *a = NU_VEC_PUSH(&p->fields);
    NU_ASSERT(p->kind == NU__SERIA_STRUCT);
    a->name   = name;
    a->count  = count;
    a->type   = fieldtype;
    a->offset = offset;
    a->flags  = flags;
}
nu_seria_type_t
nu_seria_register_enum (nu_str_t name, nu_size_t size)
{
    nu__seria_type_t *type = NU_VEC_PUSH(&_ctx.seria.types);
    type->kind             = NU__SERIA_ENUM;
    type->name             = name;
    type->size             = size;
    NU_VEC_INIT(5, &type->values);
    return NU_HANDLE_MAKE(nu_seria_type_t, _ctx.seria.types.size - 1);
}
void
nu_seria_register_enum_value (nu_seria_type_t type,
                              nu_str_t        name,
                              nu_u32_t        value)
{
    nu__seria_type_t       *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    nu__seria_enum_value_t *v = NU_VEC_PUSH(&p->values);
    NU_ASSERT(p->kind == NU__SERIA_ENUM);
    v->name  = name;
    v->value = value;
}
nu_seria_type_t
nu_seria_type (nu_str_t name)
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
                NU_INFO(NU_STR_FMT " <%d> {", NU_STR_ARGS(p->name), p->size);
                for (nu_size_t f = 0; f < p->fields.size; ++f)
                {
                    nu__seria_struct_field_t *pf = p->fields.data + f;
                    nu__seria_type_t         *subtype
                        = _ctx.seria.types.data + NU_HANDLE_INDEX(pf->type);
                    const char *flags;
                    if (pf->flags == NU_SERIA_REQUIRED)
                    {
                        flags = "REQ";
                    }
                    else
                    {
                        flags = "OPT";
                    }
                    NU_INFO("   " NU_STR_FMT " " NU_STR_FMT
                            " [%d] %s = <%d,%d>",
                            NU_STR_ARGS(pf->name),
                            NU_STR_ARGS(subtype->name),
                            pf->count,
                            flags,
                            pf->offset,
                            subtype->size);
                }
                NU_INFO("}");
                break;
            case NU__SERIA_ENUM:
                NU_INFO(NU_STR_FMT " <%d> {", NU_STR_ARGS(p->name), p->size);
                for (nu_size_t v = 0; v < p->values.size; ++v)
                {
                    nu__seria_enum_value_t *pv = p->values.data + v;
                    NU_INFO("   " NU_STR_FMT " = %d",
                            NU_STR_ARGS(pv->name),
                            pv->value);
                }
                NU_INFO("}");
                break;
        }
    }
}
static void
nu__seria_print_with_depth (nu_size_t depth, nu_str_t format, ...)
{
    va_list args;
    va_start(args, format);

    NU_STR_BUF(str, 256);
    nu_str_vfmt(str, format, args);
    NU_INFO("%*s%s", depth * 2, "", str);

    va_end(args);
}
static void
nu__seria_dump (nu_size_t       depth,
                nu_seria_type_t type,
                nu_size_t       count,
                nu_byte_t      *data)
{
    NU_ASSERT(type && data);
    nu__seria_type_t *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *ptr = (nu_byte_t *)((nu_size_t)data + i * p->size);
        switch (p->kind)
        {
            case NU__SERIA_PRIMITIVE:
                switch (p->primitive)
                {
                    case NU_SERIA_PRIMITIVE_BUF: {
                        nu_seria_buffer_t buf = *(nu_seria_buffer_t *)ptr;
                        if (buf)
                        {
                            nu__seria_print_with_depth(
                                depth, NU_STR("%d"), NU_HANDLE_INDEX(buf));
                        }
                        else
                        {
                            nu__seria_print_with_depth(depth, NU_STR("NULL"));
                        }
                    }
                    break;
                    case NU_SERIA_PRIMITIVE_U32:
                        nu__seria_print_with_depth(
                            depth, NU_STR("%d"), *(nu_u32_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_F32:
                        nu__seria_print_with_depth(
                            depth, NU_STR("%lf"), *(nu_f32_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_V3: {
                        nu_v3_t *v = (nu_v3_t *)ptr;
                        nu__seria_print_with_depth(
                            depth, NU_STR("[" NU_V3_FMT "]"), NU_V3_ARGS(*v));
                    }
                    break;
                    case NU_SERIA_PRIMITIVE_Q4: {
                        nu_q4_t *v = (nu_q4_t *)ptr;
                        nu__seria_print_with_depth(
                            depth, NU_STR("[ " NU_Q4_FMT "]"), NU_Q4_ARGS(*v));
                    }
                    break;
                }
                break;
            case NU__SERIA_STRUCT: {
                nu__seria_print_with_depth(depth, NU_STR("{"));
                for (nu_size_t f = 0; f < p->fields.size; ++f)
                {
                    nu__seria_struct_field_t *field = p->fields.data + f;
                    nu__seria_type_t         *subtype
                        = _ctx.seria.types.data + NU_HANDLE_INDEX(field->type);
                    nu__seria_print_with_depth(
                        depth + 1,
                        NU_STR(NU_STR_FMT " (" NU_STR_FMT "):"),
                        NU_STR_ARGS(field->name),
                        NU_STR_ARGS(subtype->name));
                    nu__seria_dump(depth + 2,
                                   field->type,
                                   field->count,
                                   ptr + field->offset);
                }
                nu__seria_print_with_depth(depth, NU_STR("}"));
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
                            depth, NU_STR("%s"), p->values.data[e].name);
                        found = NU_TRUE;
                        break;
                    }
                }
                if (!found)
                {
                    nu__seria_print_with_depth(depth, NU_STR("#INVALID"));
                }
            }
            break;
        }
    }
}
void
nu_seria_dump_values (nu_seria_type_t type, nu_size_t size, void *data)
{
    nu__seria_dump(0, type, size, data);
}

void
nu_seria_open_file (nu_seria_t        seria,
                    nu_seria_io_t     mode,
                    nu_seria_format_t format,
                    nu_str_t          filename)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->opened)
    {
        nu_seria_close(seria);
    }
    s->opened = NU_TRUE;
    s->mode   = mode;

    // load file
    s->bytes = nu__seria_bytes_load_file(filename, &s->bytes_size);
    NU_ASSERT(s->bytes);

    switch (s->format)
    {
        case NU_SERIA_JSON:
            nu__seria_json_open(&s->json, mode, s->bytes, s->bytes_size);
            break;
        case NU_SERIA_NBIN:
            nu__seria_nbin_open(&s->nbin, mode, s->bytes, s->bytes_size);
            break;
    }
}
void
nu_seria_open_bytes (nu_seria_t        seria,
                     nu_seria_io_t     mode,
                     nu_seria_format_t format,
                     nu_byte_t        *bytes,
                     nu_size_t         size)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->opened)
    {
        nu_seria_close(seria);
    }
    s->opened = NU_TRUE;
    s->mode   = mode;

    switch (s->format)
    {
        case NU_SERIA_JSON:
            nu__seria_json_open(&s->json, mode, bytes, size);
            break;
        case NU_SERIA_NBIN:
            nu__seria_nbin_open(&s->nbin, mode, bytes, size);
            break;
    }
}
void
nu_seria_close (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->opened)
    {
        switch (s->format)
        {
            case NU_SERIA_JSON:
                nu__seria_json_close(&s->json);
                break;
            case NU_SERIA_NBIN:
                nu__seria_nbin_close(&s->nbin);
                break;
        }
        s->opened = NU_FALSE;
        if (s->bytes)
        {
            nu_free(s->bytes, s->bytes_size);
        }
    }
}

void
nu_seria_seek (nu_seria_t seria, nu_seria_buffer_t buffer)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    NU_ASSERT(s->mode == NU_SERIA_READ);
    switch (s->format)
    {
        case NU_SERIA_JSON:
            nu__seria_json_seek(&s->json, buffer);
            break;
        case NU_SERIA_NBIN:
            nu__seria_nbin_seek(&s->nbin, buffer);
            break;
    }
}
nu_size_t
nu_seria_read (nu_seria_t      seria,
               nu_seria_type_t type,
               nu_size_t       count,
               void           *data)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    NU_ASSERT(s->mode == NU_SERIA_READ);
    switch (s->format)
    {
        case NU_SERIA_JSON:
            return nu__seria_json_read(&s->json, type, count, data);
        case NU_SERIA_NBIN:
            return nu__seria_nbin_read(&s->nbin, type, count, data);
    }
    return 0;
}
void
nu_seria_write_root (nu_seria_t seria, nu_seria_buffer_t buffer)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    NU_ASSERT(s->mode == NU_SERIA_WRITE);
    switch (s->format)
    {
        case NU_SERIA_JSON:
            // unsupported
            break;
        case NU_SERIA_NBIN:
            nu__seria_nbin_write_root(&s->nbin, buffer);
            break;
    }
}
nu_seria_buffer_t
nu_seria_write (nu_seria_t      seria,
                nu_seria_type_t type,
                nu_size_t       count,
                void           *data)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    NU_ASSERT(s->mode == NU_SERIA_WRITE);
    switch (s->format)
    {
        case NU_SERIA_JSON:
            // unsupported
            break;
        case NU_SERIA_NBIN:
            return nu__seria_nbin_write(&s->nbin, type, count, data);
    }
    return NU_NULL;
}

#endif
