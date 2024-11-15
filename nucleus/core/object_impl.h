#ifndef NU_OBJECT_IMPL_H
#define NU_OBJECT_IMPL_H

#include <nucleus/internal.h>

nu_object_type_t
nu_object_register (nu_str_t name, nu_size_t size, nu_object_handler_t handler)
{
}
nu_object_type_t
nu_object_type_find (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.core.object_types_size; ++i)
    {
        if (nu_str_eq(name, _ctx.core.object_types[i].name))
        {
            return NU_HANDLE_MAKE(nu_object_type_t, i);
        }
    }
    return NU_NULL;
}
nu_handle_t
nu_object_new (nu_object_type_t object)
{
    nu__scope_t *s = _ctx.core.scopes + NU_HANDLE_INDEX(_ctx.core.active_scope);
    nu__object_type_t *t = _ctx.core.object_types + NU_HANDLE_INDEX(object);

    if (s->data + t->size >= s->end)
    {
        NU_ERROR("out of scope memory");
        return NU_NULL;
    }

    t->handler(NU_OBJECT_CREATE, s->data);
    s->data += t->size;
}

nu_scope_t
nu_scope_new (nu_size_t size)
{
}
void
nu_scope_rewind (nu_scope_t scope)
{
}
void
nu_scope_set_active (nu_scope_t scope)
{
    NU_ASSERT(scope);
    _ctx.core.active_scope = scope;
}

#endif
