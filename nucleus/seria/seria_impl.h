#ifndef NU_SERIA_IMPL_H
#define NU_SERIA_IMPL_H

#include <nucleus/internal.h>

// #include <nucleus/seria/reflect_impl.h>
// #include <nucleus/seria/dumper_impl.h>
// #ifdef NU_BUILD_JSMN
// #include <nucleus/seria/json_impl.h>
// #endif

static void
nu__seria_register_primitive_types (void)
{
    for (nu_size_t i = 0; i < NU_SERIA_PRIMITIVE_COUNT; ++i)
    {
        nu__seria_type_t *type = NU_VEC_PUSH(&_ctx.seria.types);
        type->name             = nu_seria_primitive_names[i];
        type->is_primitive     = NU_TRUE;
        type->primitive        = i;
    }
}

static void
nu__seria_init (void)
{
    NU_POOL_INIT(1, &_ctx.seria.instances);
    NU_VEC_INIT(10, &_ctx.seria.types);
    nu__seria_register_primitive_types();
}
static void
nu__seria_free (void)
{
    for (nu_size_t i = 0; i < _ctx.seria.types.size; ++i)
    {
        NU_VEC_FREE(&_ctx.seria.types.data[i].accessors);
    }
    NU_VEC_FREE(&_ctx.seria.types);
    NU_POOL_FREE(&_ctx.seria.instances);
}

nu_seria_t
nu_seria_create (void)
{
    nu_size_t             index;
    nu__seria_instance_t *s = NU_POOL_ADD(&_ctx.seria.instances, &index);
    s->is_open              = NU_FALSE;
    return NU_HANDLE_MAKE(nu_seria_t, index);
}
void
nu_seria_delete (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    if (s->is_open)
    {
        nu_seria_close(seria);
    }
    NU_POOL_REMOVE(&_ctx.seria.instances, NU_HANDLE_INDEX(seria));
}

void
nu_seria_open_reflect (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
    s->is_open = NU_TRUE;
}
void
nu_seria_open_dumper (nu_seria_t seria)
{
}
#ifdef NU_BUILD_JSMN
void
nu_seria_open_json (nu_seria_t       seria,
                    const jsmntok_t *toks,
                    nu_size_t        toks_count)
{
}
#endif
void
nu_seria_close (nu_seria_t seria)
{
    nu__seria_instance_t *s
        = _ctx.seria.instances.data + NU_HANDLE_INDEX(seria);
}

nu_seria_type_t
nu_seria_register (const nu_char_t *name, nu_size_t size)
{
    nu__seria_type_t *layout = NU_VEC_PUSH(&_ctx.seria.types);
    layout->name             = name;
    layout->size             = size;
    layout->is_primitive     = NU_FALSE;
    NU_VEC_INIT(5, &layout->accessors);
    return NU_HANDLE_MAKE(nu_seria_type_t, _ctx.seria.types.size - 1);
}
void
nu_seria_register_field (nu_seria_type_t  type,
                         const nu_char_t *name,
                         nu_seria_type_t  fieldtype,
                         nu_size_t        size,
                         nu_size_t        offset)
{
    NU_ASSERT(fieldtype);
    NU_ASSERT(type && name && size);
    nu__seria_type_t     *p = _ctx.seria.types.data + NU_HANDLE_INDEX(type);
    nu__seria_accessor_t *a = NU_VEC_PUSH(&p->accessors);
    a->name                 = name;
    a->size                 = size;
    a->type                 = fieldtype;
    a->offset               = offset;
}
nu_seria_type_t
nu_seria_type (const nu_char_t *name)
{
    for (nu_size_t i = 0; i < _ctx.seria.types.size; ++i)
    {
        if (NU_MATCH(name, _ctx.seria.types.data[i].name))
        {
            return NU_HANDLE_MAKE(nu_seria_type_t, i);
        }
    }
    return NU_NULL;
}

#endif
