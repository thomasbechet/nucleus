#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>
#include <nucleus/seria/nbin_impl.h>

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
    nu_byte_t *bytes = (nu_byte_t *)nu_alloc(fsize);
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

#define NU__REGISTER_CORE(enum, corelayout)                            \
    {                                                                  \
        nu__seria_layout_t *layout = NU_VEC_PUSH(&_ctx.seria.layouts); \
        layout->kind               = NU__SERIA_PRIMITIVE;              \
        layout->name               = nu_seria_primitive_names[enum];   \
        layout->size               = sizeof(corelayout);               \
        layout->primitive          = enum;                             \
    }

static void
nu__seria_register_primitive_layouts (void)
{
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_BYTE, nu_byte_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_U32, nu_u32_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_F32, nu_f32_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_STR, nu_byte_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_V3, nu_v3_t);
    NU__REGISTER_CORE(NU_SERIA_PRIMITIVE_Q4, nu_q4_t);
}

#undef NU__REGISTER_CORE

static void
nu__seria_init (void)
{
    NU_POOL_INIT(1, &_ctx.seria.instances);
    NU_VEC_INIT(10, &_ctx.seria.layouts);
    nu__seria_register_primitive_layouts();
}
static void
nu__seria_free (void)
{
    for (nu_size_t i = 0; i < _ctx.seria.layouts.size; ++i)
    {
        nu__seria_layout_t *p = _ctx.seria.layouts.data + i;
        switch (p->kind)
        {
            case NU__SERIA_PRIMITIVE:
                NU_VEC_FREE(&_ctx.seria.layouts.data[i].values);
                break;
            case NU__SERIA_STRUCT:
                NU_VEC_FREE(&_ctx.seria.layouts.data[i].fields);
                break;
            case NU__SERIA_ENUM:
                break;
        }
    }
    NU_VEC_FREE(&_ctx.seria.layouts);
    NU_POOL_FREE(&_ctx.seria.instances);
}

nu_seria_t
nu_seria_create (void)
{
    nu_size_t        index;
    nu__seria_ctx_t *s = NU_POOL_ADD(&_ctx.seria.instances, &index);
    s->opened          = NU_FALSE;
    s->bytes           = NU_NULL;
    return NU_HANDLE_MAKE(nu_seria_t, index);
}
void
nu_seria_delete (nu_seria_t seria)
{
    nu__seria_ctx_t *s = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->opened)
    {
        nu_seria_close(seria);
    }
    NU_POOL_REMOVE(&_ctx.seria.instances, NU_HANDLE_INDEX(seria));
}

nu_seria_layout_t
nu_seria_register_struct (nu_str_t name, nu_size_t size)
{
    nu__seria_layout_t *layout = NU_VEC_PUSH(&_ctx.seria.layouts);
    layout->kind               = NU__SERIA_STRUCT;
    layout->name               = name;
    layout->size               = size;
    NU_VEC_INIT(5, &layout->fields);
    return NU_HANDLE_MAKE(nu_seria_layout_t, _ctx.seria.layouts.size - 1);
}
void
nu_seria_register_struct_field (nu_seria_layout_t layout,
                                nu_str_t          name,
                                nu_seria_layout_t field_layout,
                                nu_size_t         size,
                                nu_seria_flag_t   flags,
                                nu_size_t         offset)
{
    NU_ASSERT(layout);
    NU_ASSERT(field_layout);
    NU_ASSERT(size);
    nu__seria_layout_t *p = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    nu__seria_struct_field_t *a = NU_VEC_PUSH(&p->fields);
    NU_ASSERT(p->kind == NU__SERIA_STRUCT);
    a->name   = name;
    a->size   = size;
    a->layout = field_layout;
    a->offset = offset;
    a->flags  = flags;
}
nu_seria_layout_t
nu_seria_register_enum (nu_str_t name, nu_size_t size)
{
    nu__seria_layout_t *layout = NU_VEC_PUSH(&_ctx.seria.layouts);
    layout->kind               = NU__SERIA_ENUM;
    layout->name               = name;
    layout->size               = size;
    NU_VEC_INIT(5, &layout->values);
    return NU_HANDLE_MAKE(nu_seria_layout_t, _ctx.seria.layouts.size - 1);
}
void
nu_seria_register_enum_value (nu_seria_layout_t layout,
                              nu_str_t          name,
                              nu_u32_t          value)
{
    nu__seria_layout_t *p = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    nu__seria_enum_value_t *v = NU_VEC_PUSH(&p->values);
    NU_ASSERT(p->kind == NU__SERIA_ENUM);
    v->name  = name;
    v->value = value;
}
nu_seria_layout_t
nu_seria_find_layout (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.seria.layouts.size; ++i)
    {
        if (NU_MATCH(name, _ctx.seria.layouts.data[i].name))
        {
            return NU_HANDLE_MAKE(nu_seria_layout_t, i);
        }
    }
    return NU_NULL;
}
nu_str_t
nu_seria_name (nu_seria_layout_t layout)
{
    const nu__seria_layout_t *t
        = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    return t->name;
}
nu_size_t
nu_seria_size (nu_seria_layout_t layout)
{
    const nu__seria_layout_t *t
        = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    return t->size;
}

void
nu_seria_dump_layouts (void)
{
    for (nu_size_t i = 0; i < _ctx.seria.layouts.size; ++i)
    {
        nu__seria_layout_t *p = _ctx.seria.layouts.data + i;
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
                    nu__seria_layout_t       *subtype
                        = _ctx.seria.layouts.data + NU_HANDLE_INDEX(pf->layout);
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
                            pf->size,
                            flags,
                            pf->offset,
                            subtype->size * pf->size);
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
nu__seria_dump (nu_size_t         depth,
                nu_seria_layout_t layout,
                nu_size_t         size,
                nu_byte_t        *data)
{
    NU_ASSERT(layout && data);
    nu__seria_layout_t *p = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
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
                }
                break;
            case NU__SERIA_STRUCT: {
                nu__seria_print_with_depth(depth, NU_STR("{"));
                for (nu_size_t f = 0; f < p->fields.size; ++f)
                {
                    nu__seria_struct_field_t *field = p->fields.data + f;
                    nu__seria_layout_t       *subtype
                        = _ctx.seria.layouts.data
                          + NU_HANDLE_INDEX(field->layout);
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
nu__seria_write_1b (nu__seria_ctx_t *ctx, nu_byte_t v)
{
    if (ctx->ptr >= ctx->end)
    {
        NU_ERROR("invalid read");
        return;
    }
    *(nu_byte_t *)ctx->ptr = v;
    ctx->ptr += sizeof(nu_byte_t);
}
static void
nu__seria_write_4b (nu__seria_ctx_t *ctx, nu_u32_t v)
{
    if (ctx->ptr >= ctx->end)
    {
        NU_ERROR("invalid read");
        return;
    }
    *(nu_u32_t *)ctx->ptr = v;
    ctx->ptr += sizeof(nu_u32_t);
}

void
nu_seria_open_file (nu_seria_t seria, nu_seria_mode_t mode, nu_str_t filename)
{
    nu__seria_ctx_t *ctx = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (ctx->opened)
    {
        nu_seria_close(seria);
    }
    ctx->opened = NU_TRUE;
    ctx->owned  = NU_TRUE;
    ctx->mode   = mode;

    // load file
    nu_size_t size;
    ctx->bytes = nu__seria_load_bytes(filename, &size);
    NU_ASSERT(ctx->bytes);
    ctx->end = ctx->bytes + size;
    ctx->ptr = ctx->bytes;

    nu__seria_nbin_open(ctx);
}
void
nu_seria_open_bytes (nu_seria_t      seria,
                     nu_seria_mode_t mode,
                     nu_byte_t      *bytes,
                     nu_size_t       size)
{
    nu__seria_ctx_t *ctx = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (ctx->opened)
    {
        nu_seria_close(seria);
    }
    ctx->opened = NU_TRUE;
    ctx->owned  = NU_FALSE;
    ctx->mode   = mode;

    ctx->bytes = bytes;
    ctx->end   = ctx->bytes + size;
    ctx->ptr   = ctx->bytes;

    nu__seria_nbin_open(ctx);
}
nu_size_t
nu_seria_close (nu_seria_t seria)
{
    nu__seria_ctx_t *ctx = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (ctx->opened)
    {
        nu__seria_nbin_close(ctx);
        ctx->opened = NU_FALSE;
        if (ctx->owned && ctx->bytes) // free owned bytes
        {
            nu_free(ctx->bytes, ctx->end - ctx->bytes);
        }
    }
    if (ctx->mode == NU_SERIA_WRITE)
    {
        return ctx->ptr - ctx->bytes;
    }
    return 0;
}

void
nu_seria_write (nu_seria_t        seria,
                nu_seria_layout_t layout,
                nu_size_t         size,
                const void       *data)
{
    NU_ASSERT(seria);
    nu__seria_ctx_t *ctx = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    const nu__seria_layout_t *l
        = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
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
    nu__seria_ctx_t *ctx = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    const nu__seria_layout_t *l
        = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    NU_ASSERT(ctx->mode == NU_SERIA_READ);
    nu__seria_nbin_read(ctx, l, size, data);
}

#endif
