#ifndef NU_SCOPE_IMPL_H
#define NU_SCOPE_IMPL_H

#include <nucleus/internal.h>

static void
nu__scope_init (void)
{
    _ctx.core.scope.scopes_count  = 0;
    _ctx.core.scope.objects_count = 0;
    _ctx.core.scope.core_scope   = nu_scope_register(NU_STR("core"), NU_MEM_1M);
    _ctx.core.scope.active_scope = _ctx.core.scope.core_scope;
    _ctx.core.scope.last_scope   = _ctx.core.scope.core_scope;
}
static void
nu__scope_free (void)
{
    nu_scope_id_t scope = _ctx.core.scope.last_scope;
    while (scope)
    {
        nu__scope_t *s = nu__scope(scope);
        nu_free_a(&_ctx.core.allocator,
                  s->start,
                  (nu_size_t)s->end - (nu_size_t)s->start);
        scope = s->prev;
    }
}
static void
nu__scope_cleanup_all_auto (void)
{
    nu_scope_id_t scope = _ctx.core.scope.last_scope;
    while (scope)
    {
        nu__scope_t *s = nu__scope(scope);
        if (s->auto_cleanup)
        {
            nu_scope_cleanup(scope);
        }
        scope = s->prev;
    }
}
static nu__scope_t *
nu__scope (nu_scope_id_t id)
{
    NU_ASSERT(id);
    return &_ctx.core.scope.scopes[NU_ID_INDEX(id)];
}
static nu__object_type_t *
nu__object_type (nu_object_type_id_t id)
{
    NU_ASSERT(id);
    return &_ctx.core.scope.objects[NU_ID_INDEX(id)];
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
    return nu__scope(_ctx.core.scope.active_scope);
}
static void
nu__scope_cleanup_object (nu__scope_t *scope, nu__scope_header_t *header)
{
    nu__object_type_t *object = nu__object_type(header->object_type);
    if (object)
    {
        void *data
            = header + sizeof(nu__scope_header_t) + sizeof(nu__object_header_t);
        NU_DEBUG("[cleanup '" NU_STR_FMT "' obj '" NU_STR_FMT "' %p]",
                 NU_STR_ARGS(scope->name),
                 NU_STR_ARGS(object->name),
                 data);
        if (object->cleanup)
        {
            object->cleanup(data);
        }
    }
}
static nu_scope_id_t
nu__scope_register (nu_str_t name, nu_size_t size, nu_bool_t auto_cleanup)
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
    nu__scope_t *s  = &_ctx.core.scope.scopes[_ctx.core.scope.scopes_count++];
    s->name         = name;
    s->last_header  = NU_NULL;
    s->ptr          = nu_alloc_a(&_ctx.core.allocator, size);
    s->start        = s->ptr;
    s->end          = s->start + size;
    s->prev         = _ctx.core.scope.last_scope;
    s->auto_cleanup = auto_cleanup;
    nu_scope_id_t id
        = NU_ID_MAKE(nu_scope_id_t, _ctx.core.scope.scopes_count - 1);
    _ctx.core.scope.last_scope = id;
    return id;
}

nu_object_type_id_t
nu_object_register (nu_str_t name, nu_size_t size, nu_object_cleanup_t cleanup)
{
    if (_ctx.core.scope.objects_count >= NU_OBJECT_MAX)
    {
        NU_ERROR("max object type count reached");
        return NU_NULL;
    }
    if (nu_object_find_type(name))
    {
        NU_ERROR("object type already exists '" NU_STR_FMT "'",
                 NU_STR_ARGS(name));
        return NU_NULL;
    }
    NU_DEBUG("[register object '" NU_STR_FMT "' size %llu]",
             NU_STR_ARGS(name),
             size);
    nu__object_type_t *type
        = &_ctx.core.scope.objects[_ctx.core.scope.objects_count++];
    type->name    = name;
    type->size    = size;
    type->cleanup = cleanup;
    return NU_ID_MAKE(nu_object_type_id_t, _ctx.core.scope.objects_count - 1);
}
nu_object_type_id_t
nu_object_find_type (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.core.scope.objects_count; ++i)
    {
        if (nu_str_eq(name, _ctx.core.scope.objects[i].name))
        {
            return NU_ID_MAKE(nu_object_type_id_t, i);
        }
    }
    return NU_NULL;
}
nu_object_t
nu_object_new (nu_object_type_id_t type)
{
    nu__scope_t       *s = nu__scope_active();
    nu__object_type_t *t = nu__object_type(type);

    nu__scope_header_t *scope_header
        = nu__scope_alloc(s, sizeof(nu__scope_header_t));
    NU_ASSERT(scope_header);
    scope_header->object_type = type;
    scope_header->prev        = s->last_header;
    s->last_header            = scope_header;

    nu__object_header_t *object_header
        = nu__scope_alloc(s, sizeof(nu__object_header_t));
    object_header->uid = 0;
    NU_ASSERT(object_header);

    void *data = nu__scope_alloc(s, t->size);
    NU_DEBUG("[alloc '" NU_STR_FMT "' obj '" NU_STR_FMT
             "' s:%llu p:%p u:%.2lf%]",
             NU_STR_ARGS(s->name),
             NU_STR_ARGS(t->name),
             t->size,
             data,
             nu__scope_usage(s));
    return data;
}
nu_uid_t
nu_object_uid (nu_object_t obj)
{
    const nu__object_header_t *object_header
        = (const nu__object_header_t *)((const nu_byte_t *)obj
                                        - sizeof(nu__object_header_t));
    return object_header->uid;
}
nu_object_type_id_t
nu_object_type (nu_object_t obj)
{
    const nu__scope_header_t *scope_header
        = (const nu__scope_header_t *)((const nu_byte_t *)obj
                                       - sizeof(nu__object_header_t)
                                       - sizeof(nu__scope_header_t));
    return scope_header->object_type;
}
nu_object_t
nu_object_find (nu_object_type_id_t type, nu_uid_t uid)
{
    return NU_NULL;
}
void
nu_object_set_uid (nu_object_t obj, nu_uid_t uid)
{
    if (nu_object_find(nu_object_type(obj), uid))
    {
        NU_ERROR("could not set object uid from '%p' to '%p'",
                 nu_object_uid(obj),
                 uid);
    }
    nu__object_header_t *object_header
        = (nu__object_header_t *)((nu_byte_t *)obj
                                  - sizeof(nu__object_header_t));
    object_header->uid = uid;
}
nu_str_t
nu_object_type_name (nu_object_type_id_t type)
{
    nu__object_type_t *t = nu__object_type(type);
    return t->name;
}

nu_scope_id_t
nu_scope_register (nu_str_t name, nu_size_t size)
{
    return nu__scope_register(name, size, NU_TRUE);
}
void
nu_scope_cleanup (nu_scope_id_t scope)
{
    nu__scope_t *s = nu__scope(scope);

    nu__scope_header_t *header = s->last_header;
    while (header)
    {
        if (header->object_type)
        {
            nu__scope_cleanup_object(s, header);
        }
        header = header->prev;
    }

    s->last_header = NU_NULL;
    s->ptr         = s->start;
}
nu_scope_id_t
nu_scope_find (nu_str_t name)
{
    for (nu_size_t i = 0; i < _ctx.core.scope.scopes_count; ++i)
    {
        if (nu_str_eq(name, _ctx.core.scope.scopes[i].name))
        {
            return NU_ID_MAKE(nu_scope_id_t, i);
        }
    }
    return NU_NULL;
}
nu_scope_id_t
nu_scope_active (void)
{
    return _ctx.core.scope.active_scope;
}
void
nu_scope_set_active (nu_scope_id_t scope)
{
    _ctx.core.scope.active_scope = scope;
}
void
nu_scope_push (void)
{
    nu__scope_t        *s      = nu__scope_active();
    nu__scope_header_t *header = nu__scope_alloc(s, sizeof(nu__scope_header_t));
    header->object_type        = NU_NULL; // interpret as stack frame
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
        if (header->object_type)
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
nu_scope_id_t
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
