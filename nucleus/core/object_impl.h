#ifndef NU_OBJECT_IMPL_H
#define NU_OBJECT_IMPL_H

#include <nucleus/internal.h>

static void
nu__object_init (void)
{
    // register core scope
    nu__scope_t *scope  = nu__malloc(sizeof(nu__scope_t));
    scope->name         = NU_STR("core");
    scope->prev         = NU_NULL;
    scope->start        = nu__malloc(NU_MEM_1M);
    scope->ptr          = scope->start;
    scope->end          = scope->start + NU_MEM_1M;
    scope->last_header  = NU_NULL;
    scope->auto_cleanup = NU_FALSE;

    _ctx.core.object.core_scope   = (nu_scope_t)scope;
    _ctx.core.object.active_scope = _ctx.core.object.core_scope;
    _ctx.core.object.last_scope   = _ctx.core.object.core_scope;

    // register object type
    nu__object_type_t *type          = nu_malloc(sizeof(*type));
    type->size                       = sizeof(*type);
    type->load                       = NU_NULL;
    type->save                       = NU_NULL;
    type->name                       = NU_STR("object_type");
    type->cleanup                    = NU_NULL;
    _ctx.core.object.obj_object_type = (nu_object_type_t)type;

    // initialize uids
    NU_VEC_ALLOC(&_ctx.core.object.uids, 1024);

    // register scope type
    _ctx.core.object.obj_scope
        = nu_object_type_new(NU_STR("scope"), sizeof(nu__scope_t), NU_NULL);
}
static void
nu__object_free (void)
{
    nu_scope_t scope = _ctx.core.object.last_scope;
    while (scope)
    {
        nu__scope_t *s = nu__scope(scope);
        nu__free(s->start);
        scope = s->prev;
    }
}
static void
nu__scope_cleanup_all_auto (void)
{
    nu_scope_t scope = _ctx.core.object.last_scope;
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
nu__scope (nu_scope_t scope)
{
    NU_ASSERT(scope);
    return (nu__scope_t *)scope;
}
static nu__object_type_t *
nu__object_type (nu_object_type_t type)
{
    NU_ASSERT(type);
    return (nu__object_type_t *)type;
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
        NU_ERROR("scope out of memory scope=" NU_STR_FMT
                 " allocating %llu bytes",
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
    return nu__scope(_ctx.core.object.active_scope);
}
static void
nu__scope_cleanup_object (nu__scope_t *scope, nu__scope_header_t *header)
{
    nu__object_type_t *type = nu__object_type(header->type);
    if (type)
    {
        void *data = (nu_byte_t *)header + sizeof(nu__scope_header_t);
        if (nu_object_get_uid(data))
        {
            NU_DEBUG("[cleanup obj=%p type=" NU_STR_FMT " scope=" NU_STR_FMT
                     " uid=" NU_UID_FMT "]",
                     data,
                     NU_STR_ARGS(type->name),
                     NU_STR_ARGS(scope->name),
                     nu_object_get_uid(data));
        }
        else
        {
            NU_DEBUG("[cleanup obj=%p type=" NU_STR_FMT " scope=" NU_STR_FMT
                     " uid=null]",
                     data,
                     NU_STR_ARGS(type->name),
                     NU_STR_ARGS(scope->name));
        }
        if (type->cleanup)
        {
            type->cleanup(data);
        }
        if (header->flags & NU__OBJECT_HASUID)
        {
            nu_object_set_uid((nu_object_t)data, NU_NULL);
        }
    }
}
static nu_scope_t
nu__scope_new (nu_str_t name, nu_size_t size, nu_bool_t auto_cleanup)
{
    NU_DEBUG(
        "[register scope=" NU_STR_FMT " size %llu]", NU_STR_ARGS(name), size);
    nu__scope_t *s              = nu_object_new(_ctx.core.object.obj_scope);
    s->name                     = name;
    s->last_header              = NU_NULL;
    s->ptr                      = nu__malloc(size);
    s->start                    = s->ptr;
    s->end                      = s->start + size;
    s->prev                     = _ctx.core.object.last_scope;
    s->auto_cleanup             = auto_cleanup;
    _ctx.core.object.last_scope = (nu_scope_t)s;
    return (nu_scope_t)s;
}

nu_object_t
nu_object_new (nu_object_type_t type)
{
    nu__scope_t       *s = nu__scope_active();
    nu__object_type_t *t = nu__object_type(type);

    nu__scope_header_t *scope_header
        = nu__scope_alloc(s, sizeof(nu__scope_header_t));
    NU_ASSERT(scope_header);
    scope_header->type  = type;
    scope_header->prev  = s->last_header;
    scope_header->flags = 0;
    s->last_header      = scope_header;

    void *data = nu__scope_alloc(s, t->size);
    NU_DEBUG("[new addr=%p type=" NU_STR_FMT " scope=" NU_STR_FMT
             " size=%llu usage=%.2lf%]",
             data,
             NU_STR_ARGS(t->name),
             NU_STR_ARGS(s->name),
             t->size,
             data,
             nu__scope_usage(s));
    return data;
}
nu_object_t
nu_object_find (nu_object_type_t type, nu_uid_t uid)
{
    for (nu_size_t i = 0; i < _ctx.core.object.uids.size; ++i)
    {
        if (_ctx.core.object.uids.data[i].uid == uid)
        {
            nu_object_t obj = _ctx.core.object.uids.data[i].object;
            return nu_object_get_type(obj) == type ? obj : NU_NULL;
        }
    }
    return NU_NULL;
}
static nu__scope_header_t *
nu__scope_header (nu_object_t obj)
{
    return (nu__scope_header_t *)((nu_byte_t *)obj
                                  - sizeof(nu__scope_header_t));
}
nu_object_type_t
nu_object_get_type (nu_object_t obj)
{
    return nu__scope_header(obj)->type;
}
nu_uid_t
nu_object_get_uid (nu_object_t obj)
{
    nu__scope_header_t *header = nu__scope_header(obj);
    if (header->flags & NU__OBJECT_HASUID)
    {
        for (nu_size_t i = 0; i < _ctx.core.object.uids.size; ++i)
        {
            if (_ctx.core.object.uids.data[i].object == obj)
            {
                return _ctx.core.object.uids.data[i].uid;
            }
        }
        NU_UNREACHABLE();
    }
    return NU_NULL;
}
void
nu_object_set_uid (nu_object_t obj, nu_uid_t uid)
{
    if (uid)
    {
        if (nu_object_find(nu_object_get_type(obj), uid))
        {
            NU_ERROR("object uid already exists '%p'", uid);
            return;
        }
        nu__object_uid_t *t = NU_VEC_PUSH(&_ctx.core.object.uids);
        if (!t)
        {
            NU_ERROR("max uid count reached");
            return;
        }
        t->object = obj;
        t->uid    = uid;

        nu__scope_header_t *header = nu__scope_header(obj);
        header->flags              = header->flags | NU__OBJECT_HASUID;
    }
    else
    {
        nu__scope_header_t *header = nu__scope_header(obj);
        header->flags              = header->flags & ~NU__OBJECT_HASUID;
        for (nu_size_t i = 0; i < _ctx.core.object.uids.size; ++i)
        {
            if (_ctx.core.object.uids.data[i].object == obj)
            {
                NU_VEC_SWAP_REMOVE(&_ctx.core.object.uids, i);
                return;
            }
        }
        NU_UNREACHABLE();
    }
}

nu_object_type_t
nu_object_type (void)
{
    return _ctx.core.object.obj_object_type;
}
nu_object_type_t
nu_object_type_new (nu_str_t                name,
                    nu_size_t               size,
                    nu_object_cleanup_pfn_t cleanup)
{
    nu__object_type_t *type = nu_object_new(_ctx.core.object.obj_object_type);
    NU_DEBUG("[register object '" NU_STR_FMT "' size %llu]",
             NU_STR_ARGS(name),
             size);
    type->name    = name;
    type->size    = size;
    type->cleanup = cleanup;

    // Build object type uid
    nu_uid_t type_uid = nu_str_hash(name);
    nu_object_set_uid((nu_object_t)type, type_uid);

    return (nu_object_type_t)type;
}
nu_str_t
nu_object_type_name (nu_object_type_t type)
{
    return ((nu__object_type_t *)type)->name;
}
nu_object_type_t
nu_object_type_find (nu_uid_t uid)
{
    return nu_object_find(_ctx.core.object.obj_object_type, uid);
}

nu_scope_t
nu_scope_new (nu_str_t name, nu_size_t size)
{
    return nu__scope_new(name, size, NU_TRUE);
}
void
nu_scope_cleanup (nu_scope_t scope)
{
    nu__scope_t *s = nu__scope(scope);

    nu__scope_header_t *header = s->last_header;
    while (header)
    {
        if (header->type)
        {
            nu__scope_cleanup_object(s, header);
        }
        header = header->prev;
    }

    s->last_header = NU_NULL;
    s->ptr         = s->start;
}
nu_scope_t
nu_scope_active (void)
{
    return _ctx.core.object.active_scope;
}
void
nu_scope_set_active (nu_scope_t scope)
{
    _ctx.core.object.active_scope = scope;
}
void
nu_scope_push (void)
{
    nu__scope_t        *s      = nu__scope_active();
    nu__scope_header_t *header = nu__scope_alloc(s, sizeof(nu__scope_header_t));
    header->type               = NU_NULL; // interpret as stack frame
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
        if (header->type)
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
    return _ctx.core.object.core_scope;
}

void *
nu_malloc (nu_size_t size)
{
    NU_ASSERT(size);
    nu__scope_t *s = nu__scope_active();
    void        *p = nu__scope_alloc(s, size);
    NU_DEBUG("[malloc scope=" NU_STR_FMT " size=%llu usage=%.2lf%]",
             NU_STR_ARGS(s->name),
             size,
             nu__scope_usage(s));
    return p;
}

#endif
