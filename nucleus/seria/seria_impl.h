#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_JSMN
#include <nucleus/seria/json_impl.h>
#endif

static const nu_char_t *nu__seria_type_names[]
    = { "BOOL", "U32", "F32", "V3U", "V3F", "Q4", "STR", "OBJ" };

static void
nu__seria_init (void)
{
    NU_POOL_INIT(1, &_ctx.seria.instances);
}
static void
nu__seria_free (void)
{
    NU_POOL_FREE(&_ctx.seria.instances);
}

nu_seria_t
nu_seria_create (void)
{
    nu_size_t             index;
    nu__seria_instance_t *s = NU_POOL_ADD(&_ctx.seria.instances, &index);
    s->type                 = NU__SERIA_CLOSED;
    return NU_HANDLE_MAKE(nu_seria_t, index);
}
void
nu_seria_delete (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->type != NU__SERIA_CLOSED)
    {
        nu_seria_close(seria);
    }
    NU_POOL_REMOVE(&_ctx.seria.instances, NU_HANDLE_INDEX(seria));
}

void
nu_seria_open_dumper (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    s->type         = NU__SERIA_DUMPER;
    s->dumper.depth = 0;
}
#ifdef NU_BUILD_JSMN
void
nu_seria_open_json (nu_seria_t       seria,
                    const jsmntok_t *toks,
                    nu_size_t        toks_count)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    s->type = NU__SERIA_JSON;
    nu__seria_open_json(s, toks, toks_count);
}
#endif
void
nu_seria_close (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    switch (s->type)
    {
        case NU__SERIA_JSON:
            nu__seria_close_json(s);
            break;
        case NU__SERIA_DUMPER:
        case NU__SERIA_CLOSED:
            break;
    }
    s->type = NU__SERIA_CLOSED;
}

void
nu_seria_begin (nu_seria_t seria, const nu_char_t *name)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    NU_INFO("%*s%s {", s->dumper.depth * 2, "", name);
    ++s->dumper.depth;
}
void
nu_seria_end (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    --s->dumper.depth;
    NU_INFO("%*s}", s->dumper.depth * 2, "");
}
void
nu_seria_field (nu_seria_t       seria,
                const nu_char_t *name,
                nu_seria_type_t  type,
                nu_size_t        size)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (size > 2)
    {
        NU_INFO("%*s%s (%s) [%d]:",
                s->dumper.depth * 2,
                "",
                name,
                nu__seria_type_names[type],
                size);
    }
    else
    {
        NU_INFO("%*s%s (%s):",
                s->dumper.depth * 2,
                "",
                name,
                nu__seria_type_names[type]);
    }
}
void
nu_seria_data (nu_seria_t seria, void *data)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
}

#endif
