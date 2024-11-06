#ifndef NU_SERIA_NBIN_IMPL_H
#define NU_SERIA_NBIN_IMPL_H

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

static void
nu__seria_nbin_open (nu__seria_ctx_t *ctx)
{
    if (ctx->mode == NU_SERIA_READ)
    {
        // read version
        nu__seria_read_4b(ctx);
    }
    else
    {
        // write version
        nu__seria_write_4b(ctx, nu__seria_u32_le(0xFFFFFFFF));
    }
}
static void
nu__seria_nbin_close (nu__seria_ctx_t *ctx)
{
}

static void
nu__seria_nbin_write (nu__seria_ctx_t          *ctx,
                      const nu__seria_layout_t *layout,
                      nu_size_t                 size,
                      const void               *data)
{
    switch (layout->kind)
    {
        case NU__SERIA_PRIMITIVE: {
            switch (layout->primitive)
            {
                case NU_SERIA_PRIMITIVE_BYTE: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_byte_t value = *((nu_byte_t *)data + i);
                        nu__seria_write_1b(ctx, value);
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_u32_t value = *((nu_u32_t *)data + i);
                        nu__seria_write_4b(ctx, nu__seria_u32_le(value));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_f32_t value = *((nu_f32_t *)data + i);
                        nu__seria_write_4b(ctx, nu__seria_u32_le(value));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_STR: {
                    nu_byte_t *value = (nu_byte_t *)data;
                    nu_size_t  len   = nu_cstr_len(value, size);
                    nu__seria_write_4b(ctx, nu__seria_u32_le(len));
                    for (nu_size_t i = 0; i < len; ++i)
                    {
                        nu__seria_write_1b(ctx, *(value + i));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        const nu_v3_t *v = (const nu_v3_t *)data + i;
                        nu__seria_write_4b(ctx, nu__seria_u32_le(v->x));
                        nu__seria_write_4b(ctx, nu__seria_u32_le(v->y));
                        nu__seria_write_4b(ctx, nu__seria_u32_le(v->z));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        const nu_q4_t *q = (const nu_q4_t *)data + i;
                        nu__seria_write_4b(ctx, nu__seria_u32_le(q->x));
                        nu__seria_write_4b(ctx, nu__seria_u32_le(q->y));
                        nu__seria_write_4b(ctx, nu__seria_u32_le(q->z));
                        nu__seria_write_4b(ctx, nu__seria_u32_le(q->w));
                    }
                }
                break;
            }
        }
        break;
        case NU__SERIA_STRUCT: {
            for (nu_size_t i = 0; i < size; ++i)
            {
                nu_byte_t *ptr = (nu_byte_t *)data + layout->size * i;
                for (nu_size_t f = 0; f < layout->fields.size; ++f)
                {
                    const nu__seria_struct_field_t *field
                        = layout->fields.data + f;
                    const nu__seria_layout_t *sub_layout
                        = _ctx.seria.layouts.data
                          + NU_HANDLE_INDEX(field->layout);
                    nu_byte_t *data = ptr + field->offset;
                    nu__seria_nbin_write(ctx, sub_layout, field->size, data);
                }
            }
        }
        break;
        case NU__SERIA_ENUM: {
            for (nu_size_t i = 0; i < size; ++i)
            {
                nu_u32_t value
                    = *(nu_u32_t *)((nu_byte_t *)data + layout->size * i);
                for (nu_size_t v = 0; v < layout->values.size; ++v)
                {
                    if (value == layout->values.data[v].value)
                    {
                        nu_u32_t hash
                            = nu_str_hash(layout->values.data[v].name);
                        nu__seria_write_4b(ctx, nu__seria_u32_le(hash));
                        break;
                    }
                }
            }
        }
        break;
    }
}

static void
nu__seria_nbin_read (nu__seria_ctx_t          *ctx,
                     const nu__seria_layout_t *layout,
                     nu_size_t                 size,
                     void                     *data)
{
    switch (layout->kind)
    {
        case NU__SERIA_PRIMITIVE: {
            switch (layout->primitive)
            {
                case NU_SERIA_PRIMITIVE_BYTE: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_byte_t *ptr = (nu_byte_t *)data + layout->size * i;
                        *ptr           = nu__seria_read_1b(ctx);
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_u32_t *ptr = (nu_u32_t *)((nu_byte_t *)data
                                                     + layout->size * i);
                        *ptr = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_f32_t *ptr = (nu_f32_t *)((nu_byte_t *)data
                                                     + layout->size * i);
                        *ptr = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_STR: {
                    nu_u32_t length = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    NU_ASSERT(length < size);
                    nu_memset(data, 0, size);
                    for (nu_size_t i = 0; i < length; ++i)
                    {
                        *((nu_byte_t *)data + i) = nu__seria_read_1b(ctx);
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_v3_t *v
                            = (nu_v3_t *)((nu_byte_t *)data + layout->size * i);
                        v->x = nu__seria_u32_le(nu__seria_read_4b(ctx));
                        v->y = nu__seria_u32_le(nu__seria_read_4b(ctx));
                        v->z = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    for (nu_size_t i = 0; i < size; ++i)
                    {
                        nu_q4_t *q
                            = (nu_q4_t *)((nu_byte_t *)data + layout->size * i);
                        q->x = nu__seria_u32_le(nu__seria_read_4b(ctx));
                        q->y = nu__seria_u32_le(nu__seria_read_4b(ctx));
                        q->z = nu__seria_u32_le(nu__seria_read_4b(ctx));
                        q->w = nu__seria_u32_le(nu__seria_read_4b(ctx));
                    }
                }
                break;
            }
        }
        break;
        case NU__SERIA_STRUCT: {
            for (nu_size_t i = 0; i < size; ++i)
            {
                nu_byte_t *ptr = (nu_byte_t *)data + layout->size * i;
                for (nu_size_t f = 0; f < layout->fields.size; ++f)
                {
                    const nu__seria_struct_field_t *field
                        = layout->fields.data + f;
                    const nu__seria_layout_t *field_layout
                        = _ctx.seria.layouts.data
                          + NU_HANDLE_INDEX(field->layout);
                    nu_byte_t *data = ptr + field->offset;
                    nu__seria_nbin_read(ctx, field_layout, field->size, data);
                }
            }
        }
        break;
        case NU__SERIA_ENUM: {
            for (nu_size_t i = 0; i < size; ++i)
            {
                nu_byte_t *ptr   = (nu_byte_t *)data + layout->size * i;
                nu_u32_t  *value = (nu_u32_t *)ptr;
                nu_u32_t   hash  = nu__seria_u32_le(nu__seria_read_4b(ctx));
                for (nu_size_t v = 0; v < layout->values.size; ++v)
                {
                    if (hash == nu_str_hash(layout->values.data[v].name))
                    {
                        *value = layout->values.data[v].value;
                        break;
                    }
                }
            }
        }
        break;
    }
}

#endif
