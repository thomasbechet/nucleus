#ifndef NU_SERIA_H
#define NU_SERIA_H

#include <nucleus/seria/api.h>

typedef struct
{
    nu_bool_t       fileopen;
    nu_str_t        filename;
    nu_seria_mode_t mode;
    nu_byte_t      *bytes;
    nu_byte_t      *end;
    nu_byte_t      *ptr;
} nu__seria_ctx_t;

typedef struct
{
    nu_object_type_t obj_seria;
    nu_object_type_t obj_seria_struct;
    nu_object_type_t obj_seria_enum;
} nu__seria_t;

static nu_byte_t *nu__seria_load_bytes(nu_str_t filename, nu_size_t *size);
static void       nu__seria_write_bytes(nu_str_t         filename,
                                        const nu_byte_t *bytes,
                                        nu_size_t        size);

static nu_byte_t nu__seria_read_1b(nu__seria_ctx_t *ctx);
static nu_u32_t  nu__seria_read_4b(nu__seria_ctx_t *ctx);
static void      nu__seria_write_1b(nu__seria_ctx_t *ctx, nu_byte_t v);
static void      nu__seria_write_4b(nu__seria_ctx_t *ctx, nu_u32_t v);

static void nu__seria_init(void);

#endif
