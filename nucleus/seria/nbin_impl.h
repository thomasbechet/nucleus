#ifndef NU_SERIA_NBIN_IMPL_H
#define NU_SERIA_NBIN_IMPL_H

#include <nucleus/internal.h>

#define NU__NBIN_VERSION (0)
#define NU__NBIN_ROOTBUF (NU__NBIN_VERSION + 4)
#define NU__NBIN_DATA    (NU__NBIN_ROOTBUF + 4)

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
static nu_size_t
nu__nbin_read_u32 (nu_u32_t *dst, const nu_byte_t *src, nu_size_t n)
{
    const nu_u32_t *p = (const nu_u32_t *)src;
    for (nu_size_t i = 0; i < n; ++i)
    {
        *(dst + i) = nu__seria_u32_le(*(p + i));
    }
    return sizeof(nu_u32_t) * n;
}
static nu_size_t
nu__nbin_read_u64 (nu_u64_t *dst, const nu_byte_t *src, nu_size_t n)
{
    const nu_u64_t *p = (const nu_u64_t *)src;
    for (nu_size_t i = 0; i < n; ++i)
    {
        *(dst + i) = nu__seria_u64_le(*(p + i));
    }
    return sizeof(nu_u64_t) * n;
}
static nu_size_t
nu__nbin_write_u32 (nu_byte_t *dst, const nu_u32_t *src, nu_size_t n)
{
    nu_u32_t *p = (nu_u32_t *)dst;
    for (nu_size_t i = 0; i < n; ++i)
    {
        *(p + i) = nu__seria_u32_le(*(src + i));
    }
    return sizeof(nu_u32_t) * n;
}
static nu_size_t
nu__nbin_write_u64 (nu_byte_t *dst, const nu_u64_t *src, nu_size_t n)
{
    nu_u64_t *p = (nu_u64_t *)dst;
    for (nu_size_t i = 0; i < n; ++i)
    {
        *(p + i) = nu__seria_u64_le(*(src + i));
    }
    return sizeof(nu_u64_t) * n;
}

static void
nu__seria_nbin_open (nu__seria_nbin_t *nbin,
                     nu_seria_mode_t   mode,
                     nu_byte_t        *bytes,
                     nu_size_t         size)
{
    nbin->mode       = mode;
    nbin->start      = NU_NULL;
    nbin->bytes      = bytes;
    nbin->bytes_size = size;
    if (mode == NU_SERIA_READ)
    {
        // find root buffer
        nu_u32_t offset;
        nu__nbin_read_u32(&offset, nbin->bytes + NU__NBIN_ROOTBUF, 1);
        if (!offset)
        {
            NU_ERROR("missing root buffer");
            return;
        }
        nbin->it = nbin->bytes + NU__NBIN_DATA + offset;
    }
    else
    {
        nbin->it  = nbin->bytes + NU__NBIN_DATA;
        nbin->end = nbin->bytes + size;
        NU_ASSERT(nbin->it < nbin->end);

        const nu_u32_t version = 0xFFFFFFFF;
        const nu_u32_t rootbuf = 0x0;
        nu__nbin_write_u32(
            nbin->bytes + NU__NBIN_VERSION, &version, 1); // version
        nu__nbin_write_u32(
            nbin->bytes + NU__NBIN_ROOTBUF, &rootbuf, 1); // root buffer
    }
}
static nu_size_t
nu__seria_nbin_close (nu__seria_nbin_t *nbin)
{
    if (nbin->mode == NU_SERIA_WRITE)
    {
        NU_ASSERT(nbin->start);
        nu_u32_t offset = nbin->start - nbin->bytes;
        nu__nbin_write_u32(nbin->bytes + NU__NBIN_ROOTBUF, &offset, 1);
        return nbin->it - nbin->bytes;
    }
    else
    {
        return 0;
    }
}

static void
nu__seria_nbin_read_value (nu__seria_nbin_t       *nbin,
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
                    nu_u32_t           offset;
                    nbin->it += nu__nbin_read_u32(&offset, nbin->it, 1);
                    if (offset)
                    {
                        *buffer = NU_HANDLE_MAKE(nu_seria_buffer_t, offset);
                    }
                    else
                    {
                        *buffer = NU_NULL;
                    }
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    nbin->it
                        += nu__nbin_read_u32((nu_u32_t *)data, nbin->it, 1);
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    nbin->it
                        += nu__nbin_read_u32((nu_u32_t *)data, nbin->it, 1);
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    nbin->it
                        += nu__nbin_read_u32((nu_u32_t *)data, nbin->it, 3);
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    nbin->it
                        += nu__nbin_read_u32((nu_u32_t *)data, nbin->it, 4);
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
                    nu__seria_nbin_read_value(nbin, subtype, ptr);
                }
            }
        }
        break;
        case NU__SERIA_ENUM: {
            nu_u32_t *value = (nu_u32_t *)data;
            nu_u32_t  hash;
            nbin->it += nu__nbin_read_u32(&hash, nbin->it, 1);
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
nu__seria_nbin_begin_read (nu__seria_nbin_t *nbin,
                           nu_seria_type_t   type,
                           nu_seria_buffer_t buffer)
{
    // read buffer offset
    nu_u32_t offset;
    if (buffer)
    {
        offset = NU_HANDLE_INDEX(buffer);
    }
    else
    {
        nu__nbin_read_u32(&offset, nbin->bytes + NU__NBIN_ROOTBUF, 1);
        if (!offset)
        {
            NU_ERROR("trying to seek root buffer, but it was not written");
        }
    }
    NU_ASSERT(offset >= NU__NBIN_DATA);
    NU_ASSERT(offset < nbin->bytes_size);

    // read buffer size
    nu_u32_t size;
    nu__nbin_read_u32(&size, nbin->bytes + offset, 1);
    nbin->it  = nbin->bytes + offset + sizeof(nu_u32_t);
    nbin->end = nbin->it + size;
    NU_ASSERT(nbin->end < nbin->bytes + nbin->bytes_size);
    nbin->type = type;
    return size;
}
static nu_size_t
nu__seria_nbin_read (nu__seria_nbin_t *nbin, nu_size_t size, void *data)
{
    nu__seria_type_t *t = _ctx.seria.types.data + NU_HANDLE_INDEX(nbin->type);
    nu_size_t         n;
    for (n = 0; n < size; ++n)
    {
        if (nbin->it + n * t->size >= nbin->end)
        {
            break;
        }
        nu__seria_nbin_read_value(nbin, t, (nu_byte_t *)data + t->size * n);
    }
    return n;
}
static void
nu__seria_nbin_write_value (nu__seria_nbin_t       *nbin,
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
                    nbin->it += nu__nbin_write_u32(nbin->it, &offset, 1);
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    nbin->it += nu__nbin_write_u32(
                        nbin->it, (const nu_u32_t *)data, 1);
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    nbin->it += nu__nbin_write_u32(
                        nbin->it, (const nu_u32_t *)data, 1);
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    nbin->it += nu__nbin_write_u32(
                        nbin->it, (const nu_u32_t *)data, 3);
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    nbin->it += nu__nbin_write_u32(
                        nbin->it, (const nu_u32_t *)data, 4);
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
                    nu__seria_nbin_write_value(nbin, subtype, ptr);
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
                    nbin->it += nu__nbin_write_u32(nbin->it, &hash, 1);
                    break;
                }
            }
        }
        break;
    }
}
static nu_seria_buffer_t
nu__seria_nbin_begin_write (nu__seria_nbin_t *nbin, nu_seria_type_t type)
{
    // create buffer handle
    nu_seria_buffer_t buffer = NU_HANDLE_MAKE(
        nu_seria_buffer_t, (nu_size_t)nbin->it - (nu_size_t)nbin->bytes);
    nbin->start = nbin->it;
    // write buffer size
    nu_u32_t size = 0;
    nbin->it += nu__nbin_write_u32(nbin->it, &size, 1);
    nbin->type = type;
    return buffer;
}
static void
nu__seria_nbin_write (nu__seria_nbin_t *nbin, nu_size_t size, const void *data)
{
    nu__seria_type_t *t = _ctx.seria.types.data + NU_HANDLE_INDEX(nbin->type);
    // write buffer
    for (nu_size_t i = 0; i < size; ++i)
    {
        nu__seria_nbin_write_value(
            nbin, t, (const nu_byte_t *)data + t->size * i);
    }
    // update buffer size
    nu_u32_t s;
    nu__nbin_read_u32(&s, nbin->start, 1);
    s += size;
    nu__nbin_write_u32(nbin->start, &s, 1);
}

#endif
