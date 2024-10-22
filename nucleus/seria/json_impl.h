#ifndef NU_SERIA_JSON_IMPL_H
#define NU_SERIA_JSON_IMPL_H

#include <nucleus/internal.h>

static void
nu__seria_open_json (nu__seria_instance_t *s,
                     const jsmntok_t      *toks,
                     nu_size_t             toks_count)
{
    // s->json.toks       = toks;
    // s->json.toks_count = toks_count;
    // s->json.index      = 0;
}
static void
nu__seria_close_json (nu__seria_instance_t *s)
{
}

void
nu__seria_begin_json (nu__seria_instance_t *s, const nu_char_t *name)
{
}
void
nu__seria_end_json (nu__seria_instance_t *s)
{
}
nu_size_t
nu__seria_field_json (nu__seria_instance_t *s,
                      const nu_char_t      *name,
                      nu_seria_type_pfn_t   type,
                      nu_size_t             card,
                      void                 *data)
{
    return 0;
}
static void
nu__seria_core_type_json (nu__seria_instance_t *s,
                          nu__seria_primitive_t type,
                          void                 *data)
{
}

#endif
