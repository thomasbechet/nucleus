#ifndef NU_SERIA_JSON_IMPL_H
#define NU_SERIA_JSON_IMPL_H

#include <nucleus/internal.h>

static void
nu__seria_open_json (nu__seria_instance_t *s,
                     const jsmntok_t      *toks,
                     nu_size_t             toks_count)
{
    s->json.toks       = toks;
    s->json.toks_count = toks_count;
    s->json.index      = 0;
}
static void
nu__seria_close_json (nu__seria_instance_t *s)
{
}

void
nu__seria_json_begin (nu__seria_instance_t *s, const nu_char_t *name)
{
}
void
nu__seria_json_end (nu__seria_instance_t *s)
{
}
void
nu__seria_json_field (nu__seria_instance_t *s,
                      const nu_char_t      *name,
                      nu_seria_type_t       type,
                      nu_size_t             size)
{
}
void
nu__seria_json_data (nu__seria_instance_t *s, void *data)
{
}

#endif
