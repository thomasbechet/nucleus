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
static nu_u32_t
nu__nbin_read_u32 (const nu_byte_t *p)
{
    return nu__seria_u32_le(*(nu_u32_t *)p);
}
static nu_u64_t
nu__nbin_read_u64 (const nu_byte_t *p)
{
    return nu__seria_u64_le(*(nu_u64_t *)p);
}
static void
nu__nbin_write_u32 (nu_byte_t *p, nu_u32_t v)
{
    *(nu_u32_t *)p = nu__seria_u32_le(v);
}
static void
nu__nbin_write_u64 (nu_byte_t *p, nu_u64_t v)
{
    *(nu_u64_t *)p = nu__seria_u64_le(v);
}

static void
nu__seria_nbin_open (nu__seria_nbin_t *nbin,
                     nu_seria_io_t     mode,
                     nu_byte_t        *bytes,
                     nu_size_t         size)
{
    nbin->mode       = mode;
    nbin->root       = NU_NULL;
    nbin->bytes      = bytes;
    nbin->bytes_size = size;
    if (mode == NU_SERIA_READ)
    {
    }
    else
    {
        nbin->it  = nbin->bytes + NU__NBIN_DATA;
        nbin->end = nbin->bytes + size;
        NU_ASSERT(nbin->it < nbin->end);

        nu_byte_t *header = nbin->bytes;
        nu__nbin_write_u32(header + NU__NBIN_VERSION, 0xFFFFFFFF); // version
        nu__nbin_write_u32(header + NU__NBIN_ROOTBUF, 0); // root buffer
    }
}
static nu_size_t
nu__seria_nbin_close (nu__seria_nbin_t *nbin)
{
    if (nbin->mode == NU_SERIA_WRITE)
    {
        NU_ASSERT(nbin->root);
        nu_size_t offset = NU_HANDLE_INDEX(nbin->root);
        nu__nbin_write_u32(nbin->bytes + NU__NBIN_ROOTBUF, offset);
        return nbin->it - nbin->bytes;
    }
    else
    {
        return 0;
    }
}

static void
nu__seria_nbin_seek (nu__seria_nbin_t *nbin, nu_seria_buffer_t buffer)
{
    // read buffer offset
    nu_size_t offset;
    if (buffer)
    {
        offset = NU_HANDLE_INDEX(buffer);
    }
    else
    {
        offset = nu__nbin_read_u32(nbin->bytes + NU__NBIN_ROOTBUF);
        if (!offset)
        {
            NU_ERROR("trying to seek root buffer, but it was not written");
        }
    }
    NU_ASSERT(offset >= NU__NBIN_DATA);
    NU_ASSERT(offset < nbin->bytes_size);

    // read buffer size
    nu_size_t size = nu__nbin_read_u32(nbin->bytes + offset);
    nbin->it       = nbin->bytes + offset + sizeof(nu_u32_t);
    nbin->end      = nbin->it + size;
    NU_ASSERT(nbin->end < nbin->bytes + nbin->bytes_size);
}
static nu_size_t
nu__seria_nbin_read (nu__seria_nbin_t *nbin,
                     nu_seria_type_t   type,
                     nu_size_t         count,
                     void             *data)
{
    // TODO: check seek buffer defined
    return 0;
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
                    if (buffer)
                    {
                        nu__nbin_write_u32(nbin->it, NU_HANDLE_INDEX(buffer));
                    }
                    else
                    {
                        nu__nbin_write_u32(nbin->it, 0);
                    }
                    nbin->it += sizeof(nu_u32_t);
                }
                break;
                case NU_SERIA_PRIMITIVE_U32: {
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                }
                break;
                case NU_SERIA_PRIMITIVE_F32: {
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                }
                break;
                case NU_SERIA_PRIMITIVE_V3: {
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                }
                break;
                case NU_SERIA_PRIMITIVE_Q4: {
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
                    nu__nbin_write_u32(nbin->it, *(nu_u32_t *)data);
                    nbin->it += sizeof(nu_u32_t);
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
                for (nu_size_t i = 0; i < field->count; ++i)
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
                    nu__nbin_write_u32(nbin->it,
                                       nu_str_hash(t->values.data[i].name));
                    nbin->it += sizeof(nu_u32_t);
                    break;
                }
            }
        }
        break;
    }
}
static nu_seria_buffer_t
nu__seria_nbin_write (nu__seria_nbin_t *nbin,
                      nu_seria_type_t   type,
                      nu_size_t         count,
                      const void       *data)
{
    nu__seria_type_t *t = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    // create buffer handle
    nu_seria_buffer_t buffer = NU_HANDLE_MAKE(
        nu_seria_buffer_t, (nu_size_t)nbin->it - (nu_size_t)nbin->bytes);
    // write buffer count
    nu__nbin_write_u32(nbin->it, count);
    nbin->it += sizeof(nu_u32_t);
    // write buffer
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu__seria_nbin_write_value(
            nbin, t, (const nu_byte_t *)data + t->size * i);
    }
    // set buffer as root buffer
    nbin->root = buffer;
    return buffer;
}

#endif
