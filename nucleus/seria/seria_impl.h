#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/seria/nbin_impl.h>

static nu_str_t nu_seria_primitive_names[] = {
    NU_STR("byte"), NU_STR("u32"), NU_STR("str"), NU_STR("v3"), NU_STR("q4")
};

static nu_byte_t *
nu__seria_load_bytes (nu_str_t filename, nu_size_t *size)
{
    nu_byte_t fn[256];
    nu_str_to_cstr(filename, fn, 256);
    FILE *f = fopen((char *)fn, "rb");
    if (!f)
    {
        NU_ERROR("failed to open file " NU_STR_FMT, NU_STR_ARGS(filename));
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)nu_scope_alloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}
static void
nu__seria_write_bytes (nu_str_t         filename,
                       const nu_byte_t *bytes,
                       nu_size_t        size)
{
    nu_byte_t fn[256];
    nu_str_to_cstr(filename, fn, 256);
    FILE *f = fopen((char *)fn, "w");
    if (!f)
    {
        NU_ERROR("failed to open file " NU_STR_FMT, NU_STR_ARGS(filename));
        return;
    }
    fwrite(bytes, sizeof(nu_byte_t) * size, 1, f);
    fclose(f);
}

static void
nu__seria_cleanup (void *data)
{
    nu__seria_ctx_t *ctx  = data;
    nu_size_t        size = 0;
    if (ctx->mode == NU_SERIA_WRITE)
    {
        size = ctx->ptr - ctx->bytes;
        if (ctx->fileopen)
        {
            nu__seria_write_bytes(ctx->filename, ctx->bytes, size);
        }
    }
    nu__seria_nbin_close(ctx);
}

static void
nu__seria_init (void)
{
    _ctx.seria.obj_seria = nu_object_register(
        NU_OBJECT_SERIA, sizeof(nu__seria_ctx_t), nu__seria_cleanup);
    _ctx.seria.obj_seria_struct = nu_object_register(
        NU_OBJECT_SERIA_STRUCT, sizeof(nu__seria_struct_t), NU_NULL);
    _ctx.seria.obj_seria_enum = nu_object_register(
        NU_OBJECT_SERIA_ENUM, sizeof(nu__seria_enum_t), NU_NULL);
}

nu_seria_struct_t
nu_seria_register_struct (nu_str_t                       name,
                          nu_size_t                      size,
                          const nu_seria_struct_field_t *fields,
                          nu_size_t                      field_count)
{
    nu__seria_struct_t *s = nu_object_new(_ctx.seria.obj_seria_struct);
    NU_ASSERT(s);
    s->name = name;
    s->size = size;
    NU_ARRAY_ALLOC(&s->fields, field_count);
    nu_memcpy(s->fields.data, fields, sizeof(*fields) * field_count);
    return (nu_seria_struct_t)s;
}
nu_seria_struct_t
nu_seria_register_enum (nu_str_t                     name,
                        nu_size_t                    size,
                        const nu_seria_enum_value_t *values,
                        nu_size_t                    value_count)
{
    nu__seria_enum_t *e = nu_object_new(_ctx.seria.obj_seria_enum);
    NU_ASSERT(e);
    e->name = name;
    e->size = size;
    NU_ARRAY_ALLOC(&e->values, value_count);
    nu_memcpy(e->values.data, values, sizeof(*values) * value_count);
    return (nu_seria_struct_t)e;
}

nu_size_t
nu_seria_primitive_size (nu_seria_primitive_t primitive)
{
    switch (primitive)
    {
        case NU_SERIA_BYTE:
            return 1;
        case NU_SERIA_U32:
            return 4;
        case NU_SERIA_F32:
            return 4;
        case NU_SERIA_V3:
            return 4 * 3;
        case NU_SERIA_Q4:
            return 4 * 4;
    }
    return 0;
}
nu_size_t
nu_seria_struct_size (nu_seria_struct_t sstruct)
{
    return ((nu__seria_struct_t *)sstruct)->size;
}

void
nu_seria_dump_struct_type (nu_seria_struct_t sstruct)
{
    nu__seria_struct_t *s = (nu__seria_struct_t *)sstruct;
    NU_INFO("struct " NU_STR_FMT " <%d>:", NU_STR_ARGS(s->name), s->size);
    for (nu_size_t f = 0; f < s->fields.size; ++f)
    {
        const nu_seria_struct_field_t *field = s->fields.data + f;
        const char                    *flags;
        if (field->flags == NU_SERIA_REQUIRED)
        {
            flags = "REQ";
        }
        else
        {
            flags = "OPT";
        }
        nu_str_t pretype;
        nu_str_t fieldtype;
        switch (field->type.kind)
        {
            case NU_SERIA_PRIMITIVE: {
                pretype   = NU_STR("");
                fieldtype = nu_seria_primitive_names[field->type.p];
            }
            break;
            case NU_SERIA_STRUCT: {
                pretype   = NU_STR("struct");
                fieldtype = ((nu__seria_struct_t *)field->type.s)->name;
            }
            break;
            case NU_SERIA_ENUM: {
                pretype   = NU_STR("enum");
                fieldtype = ((nu__seria_enum_t *)field->type.e)->name;
            }
            break;
            case NU_SERIA_REF: {
                pretype   = NU_STR("ref");
                fieldtype = nu_object_type_name(field->type.r);
            }
            break;
            case NU_SERIA_STR: {
                pretype   = NU_STR("");
                fieldtype = NU_STR("str");
            }
            break;
        }
        NU_INFO("- " NU_STR_FMT " " NU_STR_FMT " " NU_STR_FMT
                " [%d] %s = <%d,%d>",
                NU_STR_ARGS(field->name),
                NU_STR_ARGS(pretype),
                NU_STR_ARGS(fieldtype),
                field->count,
                flags,
                field->offset,
                sizeof(nu_object_t) * field->count);
    }
}
void
nu_seria_dump_enum_type (nu_seria_enum_t senum)
{
    nu__seria_enum_t *e = (nu__seria_enum_t *)senum;
    NU_INFO("enum " NU_STR_FMT " <%d>:", NU_STR_ARGS(e->name), e->size);
    for (nu_size_t v = 0; v < e->values.size; ++v)
    {
        const nu_seria_enum_value_t *pv = e->values.data + v;
        NU_INFO("- " NU_STR_FMT " = %d", NU_STR_ARGS(pv->name), pv->value);
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
nu__seria_dump_enum (nu_size_t       depth,
                     nu_seria_enum_t senum,
                     nu_size_t       count,
                     nu_byte_t      *data)
{
    nu__seria_enum_t *e = (nu__seria_enum_t *)senum;
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *p     = data + e->size * i;
        nu_u32_t   value = *(nu_u32_t *)p;
        nu_bool_t  found = NU_FALSE;
        for (nu_size_t v = 0; v < e->values.size; ++v)
        {
            if (value == e->values.data[v].value)
            {
                nu__seria_print_with_depth(
                    depth, NU_STR("%s"), e->values.data[v].name);
                found = NU_TRUE;
                break;
            }
        }
        if (!found)
        {
            nu__seria_print_with_depth(depth, NU_STR("#INVALID"));
        }
    }
}
static void
nu__seria_dump_primitive (nu_size_t            depth,
                          nu_seria_primitive_t primitive,
                          nu_size_t            count,
                          void                *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *p
            = (nu_byte_t *)data + nu_seria_primitive_size(primitive) * i;
        switch (primitive)
        {
            case NU_SERIA_BYTE:
                nu__seria_print_with_depth(depth, NU_STR("%d"), *(nu_u8_t *)p);
                break;
            case NU_SERIA_U32:
                nu__seria_print_with_depth(depth, NU_STR("%d"), *(nu_u32_t *)p);
                break;
            case NU_SERIA_F32:
                nu__seria_print_with_depth(
                    depth, NU_STR("%lf"), *(nu_f32_t *)p);
                break;
            case NU_SERIA_V3: {
                nu_v3_t *v = (nu_v3_t *)p;
                nu__seria_print_with_depth(
                    depth, NU_STR("[" NU_V3_FMT "]"), NU_V3_ARGS(*v));
            }
            break;
            case NU_SERIA_Q4: {
                nu_q4_t *v = (nu_q4_t *)p;
                nu__seria_print_with_depth(
                    depth, NU_STR("[" NU_Q4_FMT "]"), NU_Q4_ARGS(*v));
            }
            break;
        }
    }
}
static void
nu__seria_dump_ref (nu_size_t           depth,
                    nu_object_type_id_t type,
                    nu_size_t           count,
                    nu_byte_t          *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t   *p   = (nu_byte_t *)data + sizeof(nu_object_t) * i;
        nu_object_t *obj = (nu_object_t *)p;
        if (*obj)
        {
            nu__seria_print_with_depth(
                depth, NU_STR("%llu"), nu_object_uid(obj));
        }
        else
        {
            nu__seria_print_with_depth(depth, NU_STR("null"));
        }
    }
}
static void
nu__seria_dump_str (nu_size_t  depth,
                    nu_size_t  capacity,
                    nu_size_t  count,
                    nu_byte_t *data)
{
    for (nu_size_t i = 0; i < capacity; ++i)
    {
        nu_byte_t *p   = data + i * capacity;
        nu_str_t   str = nu_str_from_cstr(p);
        nu__seria_print_with_depth(
            depth, NU_STR("'" NU_STR_FMT "'"), NU_STR_ARGS(str));
    }
}
static void
nu__seria_dump_struct (nu_size_t         depth,
                       nu_seria_struct_t sstruct,
                       nu_size_t         count,
                       nu_byte_t        *data)
{
    NU_ASSERT(sstruct && data);
    nu__seria_struct_t *p = (nu__seria_struct_t *)sstruct;
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *sdata = data + p->size * i;
        for (nu_size_t f = 0; f < p->fields.size; ++f)
        {
            nu_seria_struct_field_t *field = p->fields.data + f;
            nu_byte_t               *fdata = sdata + field->offset;
            switch (field->type.kind)
            {
                case NU_SERIA_PRIMITIVE:
                    nu__seria_dump_primitive(
                        depth + 1, field->type.p, field->count, fdata);
                    break;
                case NU_SERIA_STRUCT:
                    nu__seria_dump_struct(
                        depth + 1, field->type.s, field->count, fdata);
                    break;
                case NU_SERIA_ENUM:
                    nu__seria_dump_enum(
                        depth + 1, field->type.e, field->count, fdata);
                    break;
                case NU_SERIA_REF:
                    nu__seria_dump_ref(
                        depth + 1, field->type.r, field->count, fdata);
                    break;
                case NU_SERIA_STR:
                    nu__seria_dump_str(
                        depth + 1, field->type.c, field->count, fdata);
            }
        }
    }
}
void
nu_seria_dump_struct (nu_seria_struct_t s, nu_size_t count, void *data)
{
    nu__seria_dump_struct(0, s, count, data);
}
void
nu_seria_dump_enum (nu_seria_enum_t e, nu_size_t count, void *data)
{
    nu__seria_dump_enum(0, e, count, data);
}

static nu_byte_t
nu__seria_read_1b (nu__seria_ctx_t *ctx)
{
    if (ctx->ptr >= ctx->end)
    {
        NU_ERROR("invalid read");
        return 0;
    }
    nu_byte_t val = *(const nu_byte_t *)ctx->ptr;
    ctx->ptr += sizeof(nu_byte_t);
    return val;
}
static nu_u32_t
nu__seria_read_4b (nu__seria_ctx_t *ctx)
{
    if (ctx->ptr >= ctx->end)
    {
        NU_ERROR("invalid read");
        return 0;
    }
    nu_u32_t val = *(const nu_u32_t *)ctx->ptr;
    ctx->ptr += sizeof(nu_u32_t);
    return val;
}
static void
nu__seria_write (nu__seria_ctx_t *ctx, void *p, nu_size_t n)
{
    if (ctx->ptr + n >= ctx->end)
    {
        NU_PANIC("out of seria file buffer memory");
    }
    nu_memcpy(ctx->ptr, p, n);
    ctx->ptr += n;
}
static void
nu__seria_write_1b (nu__seria_ctx_t *ctx, nu_byte_t v)
{
    nu__seria_write(ctx, &v, sizeof(nu_byte_t));
}
static void
nu__seria_write_4b (nu__seria_ctx_t *ctx, nu_u32_t v)
{
    nu__seria_write(ctx, &v, sizeof(nu_u32_t));
}

nu_seria_t
nu_seria_new_file (nu_str_t        filename,
                   nu_seria_mode_t mode,
                   nu_size_t       buffer_size)
{
    nu__seria_ctx_t *ctx = nu_object_new(_ctx.seria.obj_seria);
    ctx->fileopen        = NU_TRUE;
    ctx->filename        = filename;
    ctx->mode            = mode;

    // load file
    if (mode == NU_SERIA_READ)
    {
        nu_size_t size;
        ctx->bytes = nu__seria_load_bytes(filename, &size);
        NU_ASSERT(ctx->bytes);
        ctx->ptr = ctx->bytes;
        ctx->end = ctx->bytes + size;
    }
    else
    {
        ctx->bytes = nu_scope_alloc(buffer_size);
        NU_ASSERT(ctx->bytes);
        ctx->ptr = ctx->bytes;
        ctx->end = ctx->bytes + buffer_size;
    }

    nu__seria_nbin_open(ctx);

    return (nu_seria_t)ctx;
}
nu_seria_t
nu_seria_new_bytes (nu_seria_mode_t mode, nu_byte_t *bytes, nu_size_t size)
{
    nu__seria_ctx_t *ctx = nu_object_new(_ctx.seria.obj_seria);
    ctx->fileopen        = NU_FALSE;
    ctx->mode            = mode;

    ctx->bytes = bytes;
    ctx->end   = ctx->bytes + size;
    ctx->ptr   = ctx->bytes;

    nu__seria_nbin_open(ctx);

    return (nu_seria_t)ctx;
}

void
nu_seria_begin (nu_seria_t seria)
{
}
void
nu_seria_end (nu_seria_t seria)
{
}

void
nu_seria_write_struct (nu_seria_t        seria,
                       nu_str_t          name,
                       nu_seria_struct_t sstruct,
                       nu_size_t         count,
                       const void       *data)
{
    NU_ASSERT(seria);
    nu__seria_ctx_t          *ctx = (nu__seria_ctx_t *)seria;
    const nu__seria_struct_t *s   = (const nu__seria_struct_t *)sstruct;
    NU_ASSERT(ctx->mode == NU_SERIA_WRITE);
    nu__seria_nbin_write_struct(ctx, name, s, count, data);
}
void
nu_seria_read_struct (nu_seria_t        seria,
                      nu_str_t          name,
                      nu_seria_struct_t sstruct,
                      nu_size_t         count,
                      void             *data)
{
    NU_ASSERT(seria);
    nu__seria_ctx_t          *ctx = (nu__seria_ctx_t *)seria;
    const nu__seria_struct_t *s   = (const nu__seria_struct_t *)sstruct;
    NU_ASSERT(ctx->mode == NU_SERIA_READ);
    nu__seria_nbin_read_struct(ctx, name, s, count, data);
}
void
nu_seria_write_enum (nu_seria_t      seria,
                     nu_str_t        name,
                     nu_seria_enum_t senum,
                     nu_size_t       count,
                     const void     *data)
{
    nu__seria_ctx_t        *ctx = (nu__seria_ctx_t *)seria;
    const nu__seria_enum_t *e   = (const nu__seria_enum_t *)senum;
    nu__seria_nbin_write_enum(ctx, name, e, count, data);
}
void
nu_seria_read_enum (nu_seria_t      seria,
                    nu_str_t        name,
                    nu_seria_enum_t senum,
                    nu_size_t       count,
                    void           *data)
{
    nu__seria_ctx_t        *ctx = (nu__seria_ctx_t *)seria;
    const nu__seria_enum_t *e   = (const nu__seria_enum_t *)senum;
    nu__seria_nbin_read_enum(ctx, name, e, count, data);
}
void
nu_seria_write_primitive (nu_seria_t           seria,
                          nu_str_t             name,
                          nu_seria_primitive_t primitive,
                          nu_size_t            count,
                          const void          *data)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_nbin_write_primitive(ctx, name, primitive, count, data);
}
void
nu_seria_read_primitive (nu_seria_t           seria,
                         nu_str_t             name,
                         nu_seria_primitive_t primitive,
                         nu_size_t            count,
                         void                *data)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_nbin_read_primitive(ctx, name, primitive, count, data);
}
void
nu_seria_write_ref (nu_seria_t         seria,
                    nu_str_t           name,
                    nu_size_t          count,
                    const nu_object_t *obj)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_nbin_write_ref(ctx, name, count, obj);
}
void
nu_seria_read_ref (nu_seria_t          seria,
                   nu_str_t            name,
                   nu_object_type_id_t type,
                   nu_size_t           count,
                   nu_object_t        *ref)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_nbin_read_ref(ctx, name, type, count, ref);
}
void
nu_seria_write_str (nu_seria_t       seria,
                    nu_str_t         name,
                    nu_size_t        capacity,
                    nu_size_t        count,
                    const nu_byte_t *buffer)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_nbin_write_str(ctx, name, capacity, count, buffer);
}
void
nu_seria_read_str (nu_seria_t seria,
                   nu_str_t   name,
                   nu_size_t  capacity,
                   nu_size_t  count,
                   nu_byte_t *buffer)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_nbin_read_str(ctx, name, capacity, count, buffer);
}

#define NU_IMPL_SERIA_PRIMITIVE(enum, ident, type)                          \
    void nu_seria_read_##ident(                                             \
        nu_seria_t seria, nu_str_t name, nu_size_t count, type *data)       \
    {                                                                       \
        nu_seria_read_primitive(seria, name, enum, count, data);            \
    }                                                                       \
    void nu_seria_write_##ident(                                            \
        nu_seria_t seria, nu_str_t name, nu_size_t count, const type *data) \
    {                                                                       \
        nu_seria_write_primitive(seria, name, enum, count, data);           \
    }                                                                       \
    type nu_seria_read_1##ident(nu_seria_t seria, nu_str_t name)            \
    {                                                                       \
        type v;                                                             \
        nu_seria_read_primitive(seria, name, enum, 1, &v);                  \
        return v;                                                           \
    }                                                                       \
    void nu_seria_write_1##ident(nu_seria_t seria, nu_str_t name, type v)   \
    {                                                                       \
        nu_seria_write_primitive(seria, name, enum, 1, &v);                 \
    }

NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_BYTE, byte, nu_byte_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_U32, u32, nu_u32_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_F32, f32, nu_f32_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_V3, v3, nu_v3_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_Q4, q4, nu_q4_t);

void
nu_seria_write_1str (nu_seria_t seria, nu_str_t name, nu_str_t str)
{
    nu_seria_write_str(seria, name, str.size, 1, str.data);
}
nu_str_t
nu_seria_read_1str (nu_seria_t seria,
                    nu_str_t   name,
                    nu_size_t  capacity,
                    nu_byte_t *buffer)
{
    nu_seria_read_str(seria, name, capacity, 1, buffer);
    return nu_str_from_cstr(buffer);
}

void
nu_object_set_seria (nu_object_type_id_t    type,
                     nu_object_seria_load_t load,
                     nu_object_seria_save_t save)
{
    nu__object_type_t *t = nu__object_type(type);
    t->load              = load;
    t->save              = save;
}
nu_object_t
nu_object_load (nu_object_type_id_t type, nu_seria_t seria)
{
    const nu__object_type_t *t = nu__object_type(type);
    NU_ASSERT(t && t->load);
    nu_object_t object = nu_object_new(type);
    t->load(seria, object);
    return object;
}
void
nu_object_save (nu_object_t object, nu_seria_t seria)
{
    const nu__object_type_t *t = nu__object_type(nu_object_type(object));
    NU_ASSERT(t && t->save);
    t->save(seria, object);
}

#endif
