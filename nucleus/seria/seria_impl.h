#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/seria/nbin_impl.h>

static nu_str_t nu_seria_primitive_names[]
    = { NU_STR("byte"), NU_STR("u32"), NU_STR("f32"),
        NU_STR("str"),  NU_STR("v3"),  NU_STR("q4") };

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

#define NU__REGISTER_CORE(enum, ident, type)                                 \
    {                                                                        \
        nu__seria_layout_t *layout                                           \
            = nu_object_new(_ctx.seria.obj_seria_layout);                    \
        layout->kind                       = NU__SERIA_PRIMITIVE;            \
        layout->name                       = nu_seria_primitive_names[enum]; \
        layout->size                       = sizeof(type);                   \
        layout->primitive                  = enum;                           \
        _ctx.seria.primitive_layouts[enum] = (nu_seria_layout_t)layout;      \
    }

static void
nu__seria_register_primitive_layouts (void)
{
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_BYTE, byte, nu_byte_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_U32, u32, nu_u32_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_F32, f32, nu_f32_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_STR, byte, nu_byte_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_V3, v3, nu_v3_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_Q4, q4, nu_q4_t);
}

#undef NU__REGISTER_CORE

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
        NU_STR(NU_SERIA), sizeof(nu__seria_ctx_t), nu__seria_cleanup);
    _ctx.seria.obj_seria_layout = nu_object_register(
        NU_STR(NU_SERIA_LAYOUT), sizeof(nu__seria_layout_t), NU_NULL);
    nu__seria_register_primitive_layouts();
}

nu_seria_layout_t
nu_seria_register_struct (nu_str_t                       name,
                          nu_size_t                      size,
                          const nu_seria_struct_field_t *fields,
                          nu_size_t                      field_count)
{
    nu__seria_layout_t *layout = nu_object_new(_ctx.seria.obj_seria_layout);
    if (!layout)
    {
        NU_ERROR("max seria layout count reached");
        return NU_NULL;
    }
    layout->kind = NU__SERIA_STRUCT;
    layout->name = name;
    layout->size = size;
    NU_ARRAY_ALLOC(&layout->fields, field_count);
    nu_memcpy(layout->fields.data, fields, sizeof(*fields) * field_count);
    return (nu_seria_layout_t)layout;
}
nu_seria_layout_t
nu_seria_register_enum (nu_str_t                     name,
                        nu_size_t                    size,
                        const nu_seria_enum_value_t *values,
                        nu_size_t                    value_count)
{
    nu__seria_layout_t *layout = nu_object_new(_ctx.seria.obj_seria_layout);
    layout->kind               = NU__SERIA_ENUM;
    layout->name               = name;
    layout->size               = size;
    NU_ARRAY_ALLOC(&layout->values, value_count);
    nu_memcpy(layout->values.data, values, sizeof(*values) * value_count);
    return (nu_seria_layout_t)layout;
}
nu_str_t
nu_seria_name (nu_seria_layout_t layout)
{
    nu__seria_layout_t *p = (nu__seria_layout_t *)layout;
    return p->name;
}
nu_size_t
nu_seria_size (nu_seria_layout_t layout)
{
    nu__seria_layout_t *p = (nu__seria_layout_t *)layout;
    return p->size;
}

void
nu_seria_dump_layout (nu_seria_layout_t layout)
{
    nu__seria_layout_t *p = (nu__seria_layout_t *)layout;
    switch (p->kind)
    {
        case NU__SERIA_PRIMITIVE:
            // don't print core types
            break;
        case NU__SERIA_STRUCT:
            NU_INFO(NU_STR_FMT " <%d> {", NU_STR_ARGS(p->name), p->size);
            for (nu_size_t f = 0; f < p->fields.size; ++f)
            {
                const nu_seria_struct_field_t *field = p->fields.data + f;
                const char                    *flags;
                if (field->flags == NU_SERIA_REQUIRED)
                {
                    flags = "REQ";
                }
                else
                {
                    flags = "OPT";
                }
                if (field->is_objref)
                {
                    NU_INFO("   " NU_STR_FMT " ref " NU_STR_FMT
                            " [%d] %s = <%d,%d>",
                            NU_STR_ARGS(field->name),
                            NU_STR_ARGS(nu_object_type_name(field->type)),
                            field->size,
                            flags,
                            field->offset,
                            sizeof(nu_object_t) * field->size);
                }
                else
                {
                    nu__seria_layout_t *subtype
                        = (nu__seria_layout_t *)field->layout;
                    NU_INFO("   " NU_STR_FMT " " NU_STR_FMT
                            " [%d] %s = <%d,%d>",
                            NU_STR_ARGS(field->name),
                            NU_STR_ARGS(subtype->name),
                            field->size,
                            flags,
                            field->offset,
                            subtype->size * field->size);
                }
            }
            NU_INFO("}");
            break;
        case NU__SERIA_ENUM:
            NU_INFO(NU_STR_FMT " <%d> {", NU_STR_ARGS(p->name), p->size);
            for (nu_size_t v = 0; v < p->values.size; ++v)
            {
                const nu_seria_enum_value_t *pv = p->values.data + v;
                NU_INFO(
                    "   " NU_STR_FMT " = %d", NU_STR_ARGS(pv->name), pv->value);
            }
            NU_INFO("}");
            break;
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
nu__seria_dump (nu_size_t         depth,
                nu_seria_layout_t layout,
                nu_size_t         size,
                nu_byte_t        *data)
{
    NU_ASSERT(layout && data);
    nu__seria_layout_t *p = (nu__seria_layout_t *)layout;
    for (nu_size_t i = 0; i < size; ++i)
    {
        nu_byte_t *ptr = (nu_byte_t *)((nu_size_t)data + i * p->size);
        switch (p->kind)
        {
            case NU__SERIA_PRIMITIVE:
                switch (p->primitive)
                {
                    case NU_SERIA_PRIMITIVE_BYTE:
                        nu__seria_print_with_depth(
                            depth, NU_STR("%d"), *(nu_u8_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_U32:
                        nu__seria_print_with_depth(
                            depth, NU_STR("%d"), *(nu_u32_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_F32:
                        nu__seria_print_with_depth(
                            depth, NU_STR("%lf"), *(nu_f32_t *)ptr);
                        break;
                    case NU_SERIA_PRIMITIVE_STR:
                        nu__seria_print_with_depth(
                            depth, NU_STR("%s"), *(nu_byte_t *)ptr);
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
                    case NU_SERIA_PRIMITIVE_COUNT:
                        NU_UNREACHABLE();
                        break;
                }
                break;
            case NU__SERIA_STRUCT: {
                nu__seria_print_with_depth(depth, NU_STR("{"));
                for (nu_size_t f = 0; f < p->fields.size; ++f)
                {
                    const nu_seria_struct_field_t *field = p->fields.data + f;
                    if (field->is_objref)
                    {
                        nu__seria_print_with_depth(
                            depth + 1,
                            NU_STR(NU_STR_FMT " (ref " NU_STR_FMT "):"),
                            NU_STR_ARGS(field->name),
                            NU_STR_ARGS(nu_object_type_name(field->type)));
                        nu_object_t *objref
                            = (nu_object_t *)((nu_byte_t *)ptr + field->offset);
                        if (*objref)
                        {
                            nu__seria_print_with_depth(depth + 2,
                                                       NU_STR("%llu"),
                                                       nu_object_uid(objref));
                        }
                        else
                        {
                            nu__seria_print_with_depth(depth + 2,
                                                       NU_STR("null"));
                        }
                    }
                    else
                    {
                        nu__seria_layout_t *subtype
                            = (nu__seria_layout_t *)field->layout;
                        nu__seria_print_with_depth(
                            depth + 1,
                            NU_STR(NU_STR_FMT " (" NU_STR_FMT "):"),
                            NU_STR_ARGS(field->name),
                            NU_STR_ARGS(subtype->name));
                        nu__seria_dump(depth + 2,
                                       field->layout,
                                       field->size,
                                       ptr + field->offset);
                    }
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
nu_seria_dump_values (nu_seria_layout_t layout, nu_size_t size, void *data)
{
    nu__seria_dump(0, layout, size, data);
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
nu_seria_write (nu_seria_t        seria,
                nu_seria_layout_t layout,
                nu_size_t         size,
                const void       *data)
{
    NU_ASSERT(seria);
    nu__seria_ctx_t          *ctx = (nu__seria_ctx_t *)seria;
    const nu__seria_layout_t *l   = (const nu__seria_layout_t *)layout;
    NU_ASSERT(ctx->mode == NU_SERIA_WRITE);
    nu__seria_nbin_write(ctx, l, size, data);
}

void
nu_seria_read (nu_seria_t        seria,
               nu_seria_layout_t layout,
               nu_size_t         size,
               void             *data)
{
    NU_ASSERT(seria);
    nu__seria_ctx_t          *ctx = (nu__seria_ctx_t *)seria;
    const nu__seria_layout_t *l   = (const nu__seria_layout_t *)layout;
    NU_ASSERT(ctx->mode == NU_SERIA_READ);
    nu__seria_nbin_read(ctx, l, size, data);
}

nu_seria_layout_t
nu_seria_primitive (nu_seria_primitive_t primitive)
{
    return _ctx.seria.primitive_layouts[primitive];
}

#define NU_IMPL_SERIA_PRIMITIVE(enum, ident, type)                             \
    void nu_seria_read_##ident(nu_seria_t seria, nu_size_t size, type *data)   \
    {                                                                          \
        nu_seria_read(seria, _ctx.seria.primitive_layouts[enum], size, data);  \
    }                                                                          \
    void nu_seria_write_##ident(                                               \
        nu_seria_t seria, nu_size_t size, const type *data)                    \
    {                                                                          \
        nu_seria_write(seria, _ctx.seria.primitive_layouts[enum], size, data); \
    }                                                                          \
    type nu_seria_read_1##ident(nu_seria_t seria)                              \
    {                                                                          \
        type v;                                                                \
        nu_seria_read(seria, _ctx.seria.primitive_layouts[enum], 1, &v);       \
        return v;                                                              \
    }                                                                          \
    void nu_seria_write_1##ident(nu_seria_t seria, type v)                     \
    {                                                                          \
        nu_seria_write(seria, _ctx.seria.primitive_layouts[enum], 1, &v);      \
    }

NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_PRIMITIVE_BYTE, byte, nu_byte_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_PRIMITIVE_U32, u32, nu_u32_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_PRIMITIVE_F32, f32, nu_f32_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_PRIMITIVE_V3, v3, nu_v3_t);
NU_IMPL_SERIA_PRIMITIVE(NU_SERIA_PRIMITIVE_Q4, q4, nu_q4_t);

void
nu_seria_write_str (nu_seria_t seria, nu_str_t str)
{
    nu_seria_write(seria,
                   _ctx.seria.primitive_layouts[NU_SERIA_PRIMITIVE_STR],
                   str.size,
                   str.data);
}
nu_str_t
nu_seria_read_str (nu_seria_t seria, nu_size_t capacity, nu_byte_t *buffer)
{
    nu_seria_read(seria,
                  _ctx.seria.primitive_layouts[NU_SERIA_PRIMITIVE_STR],
                  capacity,
                  buffer);
    return nu_str_from_cstr(buffer);
}
void
nu_seria_write_objref (nu_seria_t seria, nu_object_t objref)
{
    nu_uid_t uid = nu_object_uid(objref);
    nu_seria_write_1u32(seria, uid);
}
nu_object_t
nu_seria_read_objref (nu_seria_t seria, nu_object_type_id_t type)
{
    nu_uid_t uid = nu_seria_read_1u32(seria);
    return nu_object_find(type, uid);
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
