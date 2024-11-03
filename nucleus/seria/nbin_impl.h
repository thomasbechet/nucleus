#ifndef NU_SERIA_NBIN_IMPL_H
#define NU_SERIA_NBIN_IMPL_H

#include "nucleus/seria/seria.h"
#include <nucleus/internal.h>

static nu_u16_t
nu__seria_u16_le (nu_u16_t v)
{
    if (NU_BIG_ENDIAN)
    {
        return (v >> 8) | (v << 8);
    }
    else
    {
        return v;
    }
}
static nu_u32_t
nu__seria_u32_le (nu_u32_t v)
{
    if (NU_BIG_ENDIAN)
    {
        return ((v >> 24) & 0xff) |      // move byte 3 to byte 0
               ((v << 8) & 0xff0000) |   // move byte 1 to byte 2
               ((v >> 8) & 0xff00) |     // move byte 2 to byte 1
               ((v << 24) & 0xff000000); // byte 0 to byte 3
    }
    else
    {
        return v;
    }
}
static nu_u64_t
nu__seria_u64_le (nu_u64_t v)
{
    if (NU_BIG_ENDIAN)
    {
        v = ((v << 8) & 0xFF00FF00FF00FF00ULL)
            | ((v >> 8) & 0x00FF00FF00FF00FFULL);
        v = ((v << 16) & 0xFFFF0000FFFF0000ULL)
            | ((v >> 16) & 0x0000FFFF0000FFFFULL);
        return (v << 32) | (v >> 32);
    }
    else
    {
        return v;
    }
}

static nu_seria_buffer_t
nu__seria_nbin_read_buffer (nu__seria_ctx_t *ctx)
{
    nu_u32_t offset = nu__seria_u32_le(nu__seria_read_4b(ctx));
    if (offset)
    {
        return NU_HANDLE_MAKE(nu_seria_buffer_t, offset);
    }
    return NU_NULL;
}
static void
nu__seria_nbin_open (nu__seria_ctx_t *ctx)
{
    ctx->nbin.root_buffer = NU_NULL;
    if (ctx->mode == NU_SERIA_READ)
    {
        // read root buffer
        nu_size_t prev_offset = ctx->ptr - ctx->bytes;
        nu__seria_seek(ctx, ctx->end - ctx->bytes - sizeof(nu_u32_t));
        ctx->nbin.root_buffer = nu__seria_nbin_read_buffer(ctx);
        nu__seria_seek(ctx, prev_offset);
    }
    else
    {
        // write version
        nu__seria_write_4b(ctx, nu__seria_u32_le(12345));
    }
}
static void
nu__seria_nbin_close (nu__seria_ctx_t *ctx)
{
    if (ctx->mode == NU_SERIA_WRITE)
    {
        if (ctx->nbin.root_buffer)
        {
            nu__seria_write_4b(
                ctx, nu__seria_u32_le(NU_HANDLE_INDEX(ctx->nbin.root_buffer)));
        }
        else
        {
            nu__seria_write_4b(ctx, nu__seria_u32_le(0));
        }
    }
}

static void
nu__seria_nbin_write_value (nu__seria_ctx_t        *ctx,
                            const nu__seria_type_t *t,
                            const nu_byte_t        *data)
{
    switch (t->kind)
    {
        case NU__SERIA_PRIMITIVE: {
            switch (t->primitive)
            {
                case NU_SERIA_PRIMITIVE_BUF: {
                    nu_seria_buffer_t buffer = *(const nu_seria_buffer_t *)data;
                    nu_u32_t          offset = 0;
                    if (buffer)
                    {
                        nu_u32_t offset = NU_HANDLE_INDEX(buffer);
                    }
                    nu__seria_write_4b(ctx, nu__seria_u32_le(offset));
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    nu_u32_t value = *(nu_u32_t *)data;
                    nu__seria_write_4b(ctx, nu__seria_u32_le(value));
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    nu_f32_t value = *(nu_f32_t *)data;
                    nu__seria_write_4b(ctx, nu__seria_u32_le(value));
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    const nu_f32_t *p = (const nu_f32_t *)data;
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[0]));
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[1]));
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[2]));
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    const nu_f32_t *p = (const nu_f32_t *)data;
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[0]));
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[1]));
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[2]));
                    nu__seria_write_4b(ctx, nu__seria_u32_le(p[3]));
                }
                break;
            }
        }
        break;
        case NU__SERIA_STRUCT: {
            for (nu_size_t f = 0; f < t->fields.size; ++f)
            {
                const nu__seria_struct_field_t *field = t->fields.data + f;
                const nu__seria_type_t         *subtype
                    = _ctx.seria.types.data + NU_HANDLE_INDEX(field->type);
                for (nu_size_t i = 0; i < field->size; ++i)
                {
                    const nu_byte_t *ptr
                        = data + field->offset + i * subtype->size;
                    nu__seria_nbin_write_value(ctx, subtype, ptr);
                }
            }
        }
        break;
        case NU__SERIA_ENUM: {
            nu_u32_t value = *(nu_u32_t *)data;
            for (nu_size_t i = 0; i < t->values.size; ++i)
            {
                if (value == t->values.data[i].value)
                {
                    nu_u32_t hash = nu_str_hash(t->values.data[i].name);
                    nu__seria_write_4b(ctx, nu__seria_u32_le(hash));
                    break;
                }
            }
        }
        break;
    }
}
static nu_seria_buffer_t
nu__seria_nbin_write_begin (nu__seria_ctx_t *ctx,
                            nu_seria_type_t  type,
                            nu_size_t        size)
{
    // create buffer handle
    nu_seria_buffer_t buffer = NU_HANDLE_MAKE(
        nu_seria_buffer_t, (nu_size_t)ctx->ptr - (nu_size_t)ctx->bytes);
    // write buffer size
    nu__seria_write_4b(ctx, nu__seria_u32_le(size));
    // TODO: write type
    // TODO: write offset to previous buffer

    ctx->nbin.root_buffer = buffer;
    return buffer;
}
static void
nu__seria_nbin_write (nu__seria_ctx_t *ctx, nu_size_t size, const void *data)
{
    nu__seria_type_t *t
        = _ctx.seria.types.data + NU_HANDLE_INDEX(ctx->buf_type);
    for (nu_size_t i = 0; i < size; ++i)
    {
        nu__seria_nbin_write_value(
            ctx, t, (const nu_byte_t *)data + t->size * i);
    }
}

static void
nu__seria_nbin_read_value (nu__seria_ctx_t        *ctx,
                           const nu__seria_type_t *t,
                           nu_byte_t              *data)
{
    switch (t->kind)
    {
        case NU__SERIA_PRIMITIVE: {
            switch (t->primitive)
            {
                case NU_SERIA_PRIMITIVE_BUF: {
                    nu_seria_buffer_t *buffer = (nu_seria_buffer_t *)data;
                    *buffer                   = nu__seria_nbin_read_buffer(ctx);
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    nu_u32_t *p = (nu_u32_t *)data;
                    *p          = nu__seria_u32_le(nu__seria_read_4b(ctx));
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    nu_f32_t *p = (nu_f32_t *)data;
                    *p          = nu__seria_u32_le(nu__seria_read_4b(ctx));
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    nu_f32_t *p = (nu_f32_t *)data;
                    p[0]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    p[1]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    p[2]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    nu_f32_t *p = (nu_f32_t *)data;
                    p[0]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    p[1]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    p[2]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    p[3]        = nu__seria_u32_le(nu__seria_read_4b(ctx));
                }
                break;
            }
        }
        break;
        case NU__SERIA_STRUCT: {
            for (nu_size_t f = 0; f < t->fields.size; ++f)
            {
                const nu__seria_struct_field_t *field = t->fields.data + f;
                const nu__seria_type_t         *subtype
                    = _ctx.seria.types.data + NU_HANDLE_INDEX(field->type);
                for (nu_size_t i = 0; i < field->size; ++i)
                {
                    nu_byte_t *ptr = data + field->offset + i * subtype->size;
                    nu__seria_nbin_read_value(ctx, subtype, ptr);
                }
            }
        }
        break;
        case NU__SERIA_ENUM: {
            nu_u32_t *value = (nu_u32_t *)data;
            nu_u32_t  hash  = nu__seria_u32_le(nu__seria_read_4b(ctx));
            for (nu_size_t i = 0; i < t->values.size; ++i)
            {
                if (hash == nu_str_hash(t->values.data[i].name))
                {
                    *value = t->values.data[i].value;
                    break;
                }
            }
        }
        break;
    }
}
static nu_size_t
nu__seria_nbin_read_begin (nu__seria_ctx_t  *ctx,
                           nu_seria_type_t   type,
                           nu_seria_buffer_t buffer)
{
    // read buffer offset
    buffer          = buffer ? buffer : ctx->nbin.root_buffer;
    nu_u32_t offset = NU_HANDLE_INDEX(buffer);

    // TODO: validate offset

    nu__seria_seek(ctx, offset);

    // read buffer size
    nu_size_t size = nu__seria_u32_le(nu__seria_read_4b(ctx));
    // TODO: validate type

    return size;
}
static void
nu__seria_nbin_read (nu__seria_ctx_t *ctx, nu_size_t size, void *data)
{
    const nu__seria_type_t *t
        = _ctx.seria.types.data + NU_HANDLE_INDEX(ctx->buf_type);
    for (nu_size_t i = 0; i < size; ++i)
    {
        nu__seria_nbin_read_value(ctx, t, (nu_byte_t *)data + t->size * i);
    }
}

#endif
