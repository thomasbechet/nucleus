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
nu__nbin_seria_begin (nu__seria_ctx_t *ctx)
{
}
static void
nu__seria_nbin_end (nu__seria_ctx_t *ctx)
{
}

static void
nu__seria_nbin_write_struct (nu__seria_ctx_t    *ctx,
                             nu_str_t            name,
                             nu__seria_struct_t *s,
                             nu_size_t           count,
                             const void         *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *ptr = (nu_byte_t *)data + s->size * i;
        for (nu_size_t f = 0; f < s->fields.size; ++f)
        {
            const nu_seria_struct_field_t *field = s->fields.data + f;
            const nu_byte_t               *data  = ptr + field->offset;
            if (field->is_objref)
            {
                nu__seria_nbin_write_objref(
                    ctx, field->type, field->count, (const nu_object_t *)data);
            }
            else
            {
                const nu__seria_struct_t *subtype
                    = (const nu__seria_struct_t *)field->layout;
                nu__seria_nbin_write(ctx, subtype, field->count, data);
            }
        }
    }
}
static void
nu__seria_nbin_read_struct (nu__seria_ctx_t    *ctx,
                            nu_str_t            name,
                            nu__seria_struct_t *s,
                            nu_size_t           count,
                            void               *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *ptr = (nu_byte_t *)data + s->size * i;
        for (nu_size_t f = 0; f < s->fields.size; ++f)
        {
            const nu_seria_struct_field_t *field = s->fields.data + f;
            nu_byte_t                     *data  = ptr + field->offset;
            if (field->is_objref)
            {
                nu__seria_nbin_read_objref(
                    ctx, field->type, field->count, (nu_object_t *)data);
            }
            else
            {
                const nu__seria_struct_t *field_layout
                    = (const nu__seria_struct_t *)field->layout;
                nu__seria_nbin_read(ctx, field_layout, field->count, data);
            }
        }
    }
}
static void
nu__seria_nbin_write_enum (nu__seria_ctx_t  *ctx,
                           nu_str_t          name,
                           nu__seria_enum_t *e,
                           nu_size_t         count,
                           const void       *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_u32_t value = *(nu_u32_t *)((nu_byte_t *)data + e->size * i);
        for (nu_size_t v = 0; v < e->values.size; ++v)
        {
            if (value == e->values.data[v].value)
            {
                nu_u32_t hash = nu_str_hash(e->values.data[v].name);
                nu__seria_write_4b(ctx, nu__seria_u32_le(hash));
                break;
            }
        }
    }
}
static void
nu__seria_nbin_read_enum (nu__seria_ctx_t  *ctx,
                          nu_str_t          name,
                          nu__seria_enum_t *e,
                          nu_size_t         count,
                          void             *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t *p     = (nu_byte_t *)data + e->size * i;
        nu_u32_t  *value = (nu_u32_t *)p;
        nu_u32_t   hash  = nu__seria_u32_le(nu__seria_read_4b(ctx));
        for (nu_size_t v = 0; v < e->values.size; ++v)
        {
            if (hash == nu_str_hash(e->values.data[v].name))
            {
                *value = e->values.data[v].value;
                break;
            }
        }
    }
}
static void
nu__seria_nbin_write_primitive (nu__seria_ctx_t     *ctx,
                                nu_str_t             name,
                                nu_seria_primitive_t primitive,
                                nu_size_t            count,
                                const void          *data)
{
    switch (primitive)
    {
        case NU_SERIA_BYTE: {
            const nu_byte_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu__seria_write_1b(ctx, p[i]);
            }
        }
        break;
        case NU_SERIA_U32: {
            const nu_u32_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i]));
            }
        }
        break;
        case NU_SERIA_F32: {
            const nu_f32_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i]));
            }
        }
        break;
        case NU_SERIA_V3: {
            const nu_v3_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].x));
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].y));
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].z));
            }
        }
        break;
        case NU_SERIA_Q4: {
            const nu_q4_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].x));
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].y));
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].z));
                nu__seria_write_4b(ctx, nu__seria_u32_le(p[i].w));
            }
        }
        break;
    }
}
static void
nu__seria_nbin_read_primitive (nu__seria_ctx_t     *ctx,
                               nu_str_t             name,
                               nu_seria_primitive_t primitive,
                               nu_size_t            count,
                               void                *data)
{
    switch (primitive)
    {
        case NU_SERIA_BYTE: {
            nu_byte_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                p[i] = nu__seria_read_1b(ctx);
            }
        }
        break;
        case NU_SERIA_U32: {
            nu_u32_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                p[i] = nu__seria_u32_le(nu__seria_read_4b(ctx));
            }
        }
        break;
        case NU_SERIA_F32: {
            nu_f32_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                p[i] = nu__seria_u32_le(nu__seria_read_4b(ctx));
            }
        }
        break;
        case NU_SERIA_V3: {
            nu_v3_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                p[i].x = nu__seria_u32_le(nu__seria_read_4b(ctx));
                p[i].y = nu__seria_u32_le(nu__seria_read_4b(ctx));
                p[i].z = nu__seria_u32_le(nu__seria_read_4b(ctx));
            }
        }
        break;
        case NU_SERIA_Q4: {
            nu_q4_t *p = data;
            for (nu_size_t i = 0; i < count; ++i)
            {
                p[i].x = nu__seria_u32_le(nu__seria_read_4b(ctx));
                p[i].y = nu__seria_u32_le(nu__seria_read_4b(ctx));
                p[i].z = nu__seria_u32_le(nu__seria_read_4b(ctx));
                p[i].w = nu__seria_u32_le(nu__seria_read_4b(ctx));
            }
        }
        break;
    }
}
static void
nu__seria_nbin_write_ref (nu__seria_ctx_t   *ctx,
                          nu_str_t           name,
                          nu_size_t          count,
                          const nu_object_t *ref)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        if (ref[i])
        {
            nu__seria_write_4b(ctx, NU_NULL);
        }
        else
        {
            nu__seria_write_4b(ctx, nu_object_uid(ref[i]));
        }
    }
}
static void
nu__seria_nbin_read_ref (nu__seria_ctx_t    *ctx,
                         nu_str_t            name,
                         nu_object_type_id_t type,
                         nu_size_t           count,
                         nu_object_t        *ref)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_uid_t uid = nu__seria_read_4b(ctx);
        if (uid)
        {
            ref[i] = nu_object_find(type, uid);
        }
        else
        {
            ref[i] = NU_NULL;
        }
    }
}
static void
nu__seria_nbin_write_str (nu__seria_ctx_t *ctx, nu_str_t name, nu_str_t str)
{
    nu_byte_t *value = (nu_byte_t *)data;
    nu_size_t  len   = nu_cstr_len(value, size);
    nu__seria_write_4b(ctx, nu__seria_u32_le(len));
    for (nu_size_t i = 0; i < len; ++i)
    {
        nu__seria_write_1b(ctx, *(value + i));
    }
}
static nu_str_t
nu__seria_nbin_read_str (nu__seria_ctx_t *ctx,
                         nu_str_t         name,
                         nu_size_t        capacity,
                         nu_byte_t       *buffer)
{
    nu_u32_t length = nu__seria_u32_le(nu__seria_read_4b(ctx));
    NU_ASSERT(length < size);
    nu_memset(data, 0, size);
    for (nu_size_t i = 0; i < length; ++i)
    {
        *((nu_byte_t *)data + i) = nu__seria_read_1b(ctx);
    }
}

#endif
