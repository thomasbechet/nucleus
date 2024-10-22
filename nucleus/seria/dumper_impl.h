#ifndef NU_SERIA_DUMPER_IMPL_H
#define NU_SERIA_DUMPER_IMPL_H

#include <nucleus/internal.h>

static void
nu__print_with_depth (nu_size_t depth, const nu_char_t *format, ...)
{
    va_list args;
    va_start(args, format);

    nu_char_t str[256];
    nu_vsnprintf(str, 256, format, args);
    NU_INFO("%*s%s", depth * 2, "", str);

    va_end(args);
}

static void
nu__seria_open_dumper (nu__seria_instance_t *s)
{
    s->dumper.depth = 0;
}
static void
nu__seria_close_dumper (nu__seria_instance_t *s)
{
}

static void
nu__seria_begin_dumper (nu__seria_instance_t *s, const nu_char_t *name)
{
    // NU_INFO("%*s%s {", s->dumper.depth * 2, "", name);
    nu__print_with_depth(s->dumper.depth, "{");
    ++s->dumper.depth;
}
static void
nu__seria_end_dumper (nu__seria_instance_t *s)
{
    nu__print_with_depth(s->dumper.depth, "}");
    --s->dumper.depth;
}
nu_size_t
nu__seria_field_dumper (nu__seria_instance_t *s,
                        const nu_char_t      *name,
                        nu_seria_type_pfn_t   type,
                        nu_size_t             card,
                        void                 *data)
{
    nu__print_with_depth(s->dumper.depth, "%s <%s,%d>", name);
    return 0;
}
static void
nu__seria_core_type_dumper (nu__seria_instance_t *s,
                            nu__seria_primitive_t type,
                            void                 *data)
{
    switch (type)
    {
        case NU__SERIA_BOOL:
        case NU__SERIA_F32:
        case NU__SERIA_U32:
            break;
    }
}

#endif
