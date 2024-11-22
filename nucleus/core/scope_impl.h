#ifndef NU_SCOPE_IMPL_H
#define NU_SCOPE_IMPL_H

#include <nucleus/internal.h>

static void
nu__scope_init (void)
{
    _ctx.core.scope.scopes_count  = 0;
    _ctx.core.scope.objects_count = 0;
    _ctx.core.scope.last_scope    = NU_NULL;
    _ctx.core.scope.core_scope   = nu_scope_register(NU_STR("core"), NU_MEM_1M);
    _ctx.core.scope.active_scope = _ctx.core.scope.core_scope;
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
static nu_f32_t
nu__scope_usage (const nu__scope_t *scope)
{
    nu_size_t usage    = (nu_size_t)scope->ptr - (nu_size_t)scope->start;
    nu_size_t capacity = (nu_size_t)scope->end - (nu_size_t)scope->start;
    return (nu_f32_t)usage / (nu_f32_t)capacity * 100.0;
}
static void *
nu__scope_alloc (nu__scope_t *scope, nu_size_t size)
{
    if (scope->ptr + size >= scope->end)
    {
        NU_ERROR("scope out of memory '" NU_STR_FMT "' allocating %llu bytes",
                 NU_STR_ARGS(scope->name),
                 size);
        return NU_NULL;
    }
    nu_byte_t *p = scope->ptr;
    scope->ptr += size;
    return p;
}
static nu__scope_t *
nu__scope_active (void)
{
    NU_ASSERT(_ctx.core.scope.active_scope);
    return (nu__scope_t *)_ctx.core.scope.active_scope;
}
static void
nu__scope_cleanup_object (nu__scope_t *scope, nu__scope_header_t *header)
{
    nu__object_t *object = (nu__object_t *)header->object;
    if (object)
    {
        NU_DEBUG("[cleanup '" NU_STR_FMT "' obj '" NU_STR_FMT "' %p]",
                 NU_STR_ARGS(scope->name),
                 NU_STR_ARGS(object->name),
                 header + 1);
        object->handler(NU_OBJECT_CLEANUP, header + 1);
    }
}

nu_object_t
nu_object_register (nu_str_t name, nu_size_t size, nu_object_handler_t handler)
{
    if (_ctx.core.scope.objects_count >= NU_OBJECT_MAX)
    {
        NU_ERROR("max object type count reached");
        return NU_NULL;
    }
    if (nu_object_find(name))
    {
        NU_ERROR("object type already exists '" NU_STR_FMT "'",
                 NU_STR_ARGS(name));
        return NU_NULL;
    }
    NU_DEBUG("[register object '" NU_STR_FMT "' size %llu]",
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
nu_object_find (nu_str_t name)
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
void *
nu_object_new (nu_object_t type)
{
    nu__scope_t  *s = nu__scope_active();
    nu__object_t *t = (nu__object_t *)type;

    nu__scope_header_t *header = nu__scope_alloc(s, sizeof(nu__scope_header_t));
    header->object             = t;
    header->prev               = s->last_header;
    s->last_header             = header;

    void *obj = nu__scope_alloc(s, t->size);
    NU_DEBUG("[alloc '" NU_STR_FMT "' obj '" NU_STR_FMT
             "' s:%llu p:%p u:%.2lf%]",
             NU_STR_ARGS(s->name),
             NU_STR_ARGS(t->name),
             t->size,
             obj,
             nu__scope_usage(s));
    return obj;
}

nu_scope_t
nu_scope_register (nu_str_t name, nu_size_t size)
{
    if (_ctx.core.scope.scopes_count >= NU_SCOPE_MAX)
    {
        NU_ERROR("max scope count reached");
        return NU_NULL;
    }
    if (nu_scope_find(name))
    {
        NU_ERROR("scope already exists '" NU_STR_FMT "'", NU_STR_ARGS(name));
        return NU_NULL;
    }
    NU_DEBUG(
        "[register scope '" NU_STR_FMT "' size %llu]", NU_STR_ARGS(name), size);
    nu__scope_t *s = &_ctx.core.scope.scopes[_ctx.core.scope.scopes_count++];
    s->name        = name;
    s->last_header = NU_NULL;
    s->ptr         = nu_alloc(size);
    s->start       = s->ptr;
    s->end         = s->start + size;
    s->prev        = _ctx.core.scope.last_scope;
    _ctx.core.scope.last_scope = (nu_scope_t)s;
    return (nu_scope_t)s;
}
void
nu_scope_cleanup (nu_scope_t scope)
{
    nu__scope_t *s = (nu__scope_t *)scope;

    nu__scope_header_t *header = s->last_header;
    while (header)
    {
        if (header->object)
        {
            nu__scope_cleanup_object(s, header);
        }
        header = header->prev;
    }

    s->last_header = NU_NULL;
    s->ptr         = s->start;
}
nu_scope_t
nu_scope_find (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.core.scope.scopes_count; ++i)
    {
        if (nu_str_eq(name, _ctx.core.scope.scopes[i].name))
        {
            return (nu_scope_t)(_ctx.core.scope.scopes + i);
        }
    }
    return NU_NULL;
}
nu_scope_t
nu_scope_active (void)
{
    return _ctx.core.scope.active_scope;
}
void
nu_scope_set_active (nu_scope_t scope)
{
    _ctx.core.scope.active_scope = scope;
}
void
nu_scope_push (void)
{
    nu__scope_t        *s      = nu__scope_active();
    nu__scope_header_t *header = nu__scope_alloc(s, sizeof(nu__scope_header_t));
    header->object             = NU_NULL; // interpret as stack frame
    header->prev               = s->last_header;
    s->last_header             = header;
}
void
nu_scope_pop (void)
{
    nu__scope_t        *s      = nu__scope_active();
    nu__scope_header_t *header = s->last_header;
    while (header)
    {
        if (header->object)
        {
            nu__scope_cleanup_object(s, header);
        }
        else
        {
            // reach end of the frame
            s->last_header = header->prev;
            s->ptr         = (nu_byte_t *)header;
            break;
        }
        header = header->prev;
    }
}
nu_scope_t
nu_scope_core (void)
{
    return _ctx.core.scope.core_scope;
}

void *
nu_scope_alloc (nu_size_t size)
{
    NU_ASSERT(size);
    nu__scope_t *s = nu__scope_active();
    void        *p = nu__scope_alloc(s, size);
    NU_DEBUG("[alloc '" NU_STR_FMT "' raw s:%llu p:%p u:%.2lf%]",
             NU_STR_ARGS(s->name),
             size,
             p,
             nu__scope_usage(s));
    return p;
}

#endif
