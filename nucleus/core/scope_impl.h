#ifndef NU_SCOPE_IMPL_H
#define NU_SCOPE_IMPL_H

#include <nucleus/internal.h>

static void
nu__scope_init (void)
{
    _ctx.core.scope.scopes_count  = 0;
    _ctx.core.scope.objects_count = 0;
    _ctx.core.scope.active_scope  = NU_NULL;
    _ctx.core.scope.last_scope    = NU_NULL;
}
static void
nu__scope_cleanup_all (void)
{
    nu_scope_t scope = _ctx.core.scope.last_scope;
    while (scope)
    {
        nu_scope_cleanup(scope);
        nu__scope_t *s = (nu__scope_t *)scope;
        nu_free(s->start, (nu_size_t)s->end - (nu_size_t)s->start);
        scope = s->prev;
    }
}

static void *
nu__scope_alloc (nu__scope_t *scope, nu_size_t size)
{
    if (scope->ptr + size >= scope->end)
    {
        return NU_NULL;
    }
    nu_byte_t *p = scope->ptr;
    scope->ptr += size;
    return p;
}

nu_object_t
nu_object_register (nu_str_t name, nu_size_t size, nu_object_handler_t handler)
{
    if (_ctx.core.scope.objects_count >= NU_OBJECT_MAX)
    {
        NU_ERROR("max object type count reached");
        return NU_NULL;
    }
    if (nu_object_type_find(name))
    {
        NU_ERROR("object type already exists '" NU_STR_FMT "'",
                 NU_STR_ARGS(name));
        return NU_NULL;
    }
    NU_INFO("[register object '" NU_STR_FMT "' size %llu]",
            NU_STR_ARGS(name),
            size);
    nu__object_t *type
        = &_ctx.core.scope.objects[_ctx.core.scope.objects_count++];
    type->name    = name;
    type->size    = size;
    type->handler = handler;
    return (nu_object_t)type;
}
nu_object_t
nu_object_type_find (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.core.scope.objects_count; ++i)
    {
        if (nu_str_eq(name, _ctx.core.scope.objects[i].name))
        {
            return (nu_object_t)(_ctx.core.scope.objects + i);
        }
    }
    return NU_NULL;
}

nu_scope_t
nu_scope_register (nu_str_t name, nu_size_t size)
{
    if (_ctx.core.scope.scopes_count >= NU_SCOPE_MAX)
    {
        NU_ERROR("max scope count reached");
        return NU_NULL;
    }
    NU_INFO(
        "[register scope '" NU_STR_FMT "' size %llu]", NU_STR_ARGS(name), size);
    nu__scope_t *s = &_ctx.core.scope.scopes[_ctx.core.scope.scopes_count++];
    s->name        = name;
    s->last_object = NU_NULL;
    s->ptr         = nu_alloc(size);
    s->start       = s->ptr;
    s->end         = s->start + size;
    s->prev        = _ctx.core.scope.last_scope;
    _ctx.core.scope.last_scope = (nu_scope_t)s;
    return (nu_scope_t)s;
}
void *
nu_scope_alloc (nu_size_t size)
{
    nu__scope_t *s = (nu__scope_t *)_ctx.core.scope.active_scope;
    NU_INFO("[alloc %llu scope '" NU_STR_FMT "']", size, NU_STR_ARGS(s->name));
    return nu__scope_alloc(s, size);
}
void *
nu_scope_new (nu_object_t type)
{
    nu__scope_t  *s = (nu__scope_t *)_ctx.core.scope.active_scope;
    nu__object_t *t = (nu__object_t *)type;

    nu__object_header_t *header
        = nu__scope_alloc(s, sizeof(nu__object_header_t));
    header->type   = type;
    header->prev   = s->last_object;
    s->last_object = header;

    void *obj = nu__scope_alloc(s, t->size);
    NU_INFO("[new '" NU_STR_FMT "' %p scope '" NU_STR_FMT "']",
            NU_STR_ARGS(t->name),
            obj,
            NU_STR_ARGS(s->name));
    t->handler(NU_OBJECT_NEW, obj);
    return obj;
}
void
nu_scope_cleanup (nu_scope_t scope)
{
    nu__scope_t *s = (nu__scope_t *)_ctx.core.scope.active_scope;

    nu__object_header_t *header = s->last_object;
    while (header)
    {
        nu__object_t *t = (nu__object_t *)header->type;
        NU_INFO("[cleanup '" NU_STR_FMT "' %p scope '" NU_STR_FMT "']",
                NU_STR_ARGS(t->name),
                header + 1,
                NU_STR_ARGS(s->name));
        t->handler(NU_OBJECT_CLEANUP, header + 1);
        header = header->prev;
    }

    s->last_object = NU_NULL;
    s->ptr         = s->start;
}
void
nu_scope_set_active (nu_scope_t scope)
{
    NU_ASSERT(scope);
    _ctx.core.scope.active_scope = scope;
}

#endif
