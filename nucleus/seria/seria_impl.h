#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

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
    nu_byte_t *bytes = (nu_byte_t *)nu_malloc(fsize);
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
}

static void
nu__seria_init (void)
{
    _ctx.seria.obj_seria = nu_object_type_new(
        NU_STR("seria"), sizeof(nu__seria_ctx_t), nu__seria_cleanup);
}

static void
nu__seria_read (nu__seria_ctx_t *ctx, nu_size_t n, nu_byte_t *p)
{
    if (ctx->ptr + n >= ctx->end)
    {
        NU_PANIC("invalid read");
    }
    nu_memcpy(p, ctx->ptr, n);
    ctx->ptr += n;
}
static void
nu__seria_write (nu__seria_ctx_t *ctx, nu_size_t n, const nu_byte_t *p)
{
    if (ctx->ptr + n >= ctx->end)
    {
        NU_PANIC("out of seria file buffer memory");
    }
    nu_memcpy(ctx->ptr, p, n);
    ctx->ptr += n;
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
        ctx->bytes = nu_malloc(buffer_size);
        NU_ASSERT(ctx->bytes);
        ctx->ptr = ctx->bytes;
        ctx->end = ctx->bytes + buffer_size;
    }

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

    return (nu_seria_t)ctx;
}

void
nu_seria_read_byte (nu_seria_t seria, nu_size_t count, nu_byte_t *p)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_read(ctx, count, p);
}
void
nu_seria_write_byte (nu_seria_t seria, nu_size_t count, const nu_byte_t *p)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    nu__seria_write(ctx, count, p);
}
void
nu_seria_read_u32 (nu_seria_t seria, nu_size_t count, nu_u32_t *p)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_u32_t b;
        nu__seria_read(ctx, sizeof(b), (nu_byte_t *)&b);
        b    = nu__seria_u32_le(b);
        p[i] = b;
    }
}
void
nu_seria_write_u32 (nu_seria_t seria, nu_size_t count, const nu_u32_t *p)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_u32_t b = nu__seria_u32_le(p[i]);
        nu__seria_write(ctx, sizeof(b), (nu_byte_t *)&b);
    }
}
void
nu_seria_read_f32 (nu_seria_t seria, nu_size_t count, nu_f32_t *p)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_u32_t b;
        nu__seria_read(ctx, sizeof(b), (nu_byte_t *)&b);
        b = nu__seria_u32_le(b);
        nu_memcpy(p + i, &b, sizeof(b));
    }
}
void
nu_seria_write_f32 (nu_seria_t seria, nu_size_t count, const nu_f32_t *p)
{
    nu__seria_ctx_t *ctx = (nu__seria_ctx_t *)seria;
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_u32_t b;
        nu_memcpy(&b, p + i, sizeof(b));
        b = nu__seria_u32_le(b);
        nu__seria_write(ctx, sizeof(b), (nu_byte_t *)&b);
    }
}
void
nu_seria_read_v3 (nu_seria_t seria, nu_size_t count, nu_v3_t *p)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_seria_read_f32(seria, NU_V3_SIZE, p[i].data);
    }
}
void
nu_seria_write_v3 (nu_seria_t seria, nu_size_t count, const nu_v3_t *p)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_seria_write_f32(seria, NU_V3_SIZE, p[i].data);
    }
}
void
nu_seria_read_q4 (nu_seria_t seria, nu_size_t count, nu_q4_t *p)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_seria_read_f32(seria, NU_Q4_SIZE, p[i].data);
    }
}
void
nu_seria_write_q4 (nu_seria_t seria, nu_size_t count, const nu_q4_t *p)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_seria_write_f32(seria, NU_Q4_SIZE, p[i].data);
    }
}
void
nu_seria_read_m4 (nu_seria_t seria, nu_size_t count, nu_m4_t *p)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_seria_read_f32(seria, NU_M4_SIZE, p[i].data);
    }
}
void
nu_seria_write_m4 (nu_seria_t seria, nu_size_t count, const nu_m4_t *p)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_seria_write_f32(seria, NU_M4_SIZE, p[i].data);
    }
}
nu_object_t
nu_seria_read_object (nu_seria_t seria, nu_object_type_t type)
{
    nu_uid_t uid;
    nu_seria_read_u32(seria, 1, &uid);
    return nu_object_find(type, uid);
}
void
nu_seria_write_object (nu_seria_t seria, nu_object_t obj)
{
    nu_uid_t uid = nu_object_get_uid(obj);
    nu_seria_write_u32(seria, 1, &uid);
}
nu_str_t
nu_seria_read_str (nu_seria_t seria, nu_size_t capacity, nu_byte_t *buffer)
{
    nu_u32_t s;
    nu_seria_read_u32(seria, 1, &s);
    NU_ASSERT(s <= capacity);
    nu_memset(buffer, 0, capacity);
    nu_seria_read_byte(seria, s, buffer);
    return nu_str_from_cstr(buffer);
}
void
nu_seria_write_str (nu_seria_t seria, nu_str_t str)
{
    nu_u32_t s = str.size;
    nu_seria_write_u32(seria, 1, &s);
    nu_seria_write_byte(seria, s, str.data);
}

void
nu_object_set_seria (nu_object_type_t       type,
                     nu_seria_load_object_t load,
                     nu_seria_save_object_t save)
{
    nu__object_type_t *t = nu__object_type(type);
    t->load              = load;
    t->save              = save;
}
nu_object_t
nu_seria_load_object (nu_seria_t seria, nu_object_type_t type)
{
    const nu__object_type_t *t = nu__object_type(type);
    NU_ASSERT(t && t->load);
    nu_object_t object = nu_object_new(type);
    t->load(seria, object);
    return object;
}
void
nu_seria_save_object (nu_seria_t seria, nu_object_t object)
{
    const nu__object_type_t *t = nu__object_type(nu_object_get_type(object));
    NU_ASSERT(t && t->save);
    t->save(seria, object);
}

#endif
