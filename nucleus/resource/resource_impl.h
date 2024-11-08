#ifndef NU_RESOURCE_IMPL_H
#define NU_RESOURCE_IMPL_H

#include <nucleus/internal.h>

static nu_error_t
nu__resource_init (void)
{
    NU_VEC_INIT(10, &_ctx.resource.types);
    NU_POOL_INIT(10, &_ctx.resource.entries);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__resource_free (void)
{
    for (nu_size_t i = 0; i < _ctx.resource.entries.capacity; ++i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
        if (res->used)
        {
            nu_resource_remove(NU_HANDLE_MAKE(nu_resource_t, i));
        }
    }
    NU_POOL_FREE(&_ctx.resource.entries);
    NU_VEC_FREE(&_ctx.resource.types);
    return NU_ERROR_NONE;
}

nu_resource_type_t
nu_resource_register (nu_uid_t               uid,
                      nu_resource_load_pfn_t load,
                      nu_resource_save_pfn_t save,
                      nu_resource_free_pfn_t free)
{
    if (nu_resource_type_find(uid))
    {
        NU_ERROR("resource type %d already exists", uid);
        return NU_NULL;
    }

    nu__resource_type_t *t = NU_VEC_PUSH(&_ctx.resource.types);
    t->uid                 = uid;
    t->load                = load;
    t->save                = save;
    t->free                = free;
    return NU_HANDLE_MAKE(nu_resource_type_t, _ctx.resource.types.size - 1);
}
nu_resource_type_t
nu_resource_type_find (nu_uid_t uid)
{
    for (nu_size_t i = 0; i < _ctx.resource.types.size; ++i)
    {
        const nu__resource_type_t *t = _ctx.resource.types.data + i;
        if (t->uid == uid)
        {
            return NU_HANDLE_MAKE(nu_resource_type_t, i);
        }
    }
    return NU_NULL;
}

nu_resource_t
nu_resource_add (nu_resource_type_t type, nu_uid_t uid, void *data)
{
    NU_ASSERT(type);
    NU_ASSERT(data);

    const nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(type);

    nu_size_t             index;
    nu__resource_entry_t *res = NU_POOL_ADD(&_ctx.resource.entries, &index);
    res->used                 = NU_TRUE;
    res->uid                  = uid;
    res->type                 = type;
    res->data                 = data;
    res->refcount             = 0;
    return NU_HANDLE_MAKE(nu_resource_t, index);
}
void
nu_resource_remove (nu_resource_t resource)
{
    NU_ASSERT(resource);

    nu__resource_entry_t *res
        = _ctx.resource.entries.data + NU_HANDLE_INDEX(resource);
    const nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(res->type);

    NU_ASSERT(res->refcount == 0);

    // free resource
    if (res->data && t->free)
    {
        t->free(res->data);
    }

    res->data = NU_NULL;
    res->used = NU_FALSE;
}
nu_resource_t
nu_resource_find (nu_uid_t uid)
{
    for (nu_size_t i = 0; i < _ctx.resource.types.size; ++i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
        if (res->uid == uid)
        {
            return NU_HANDLE_MAKE(nu_resource_t, i);
        }
    }
    return NU_NULL;
}
void
nu_resource_acquire (nu_resource_t resource)
{
    NU_ASSERT(resource);
    ++_ctx.resource.entries.data[NU_HANDLE_INDEX(resource)].refcount;
}
void
nu_resource_release (nu_resource_t resource)
{
    NU_ASSERT(resource);
    NU_ASSERT(_ctx.resource.entries.data[NU_HANDLE_INDEX(resource)].refcount);
    --_ctx.resource.entries.data[NU_HANDLE_INDEX(resource)].refcount;
}
void *
nu_resource_data (nu_resource_t resource)
{
    NU_ASSERT(resource);
    return _ctx.resource.entries.data[NU_HANDLE_INDEX(resource)].data;
}
nu_resource_type_t
nu_resource_get_type (nu_resource_t resource)
{
    NU_ASSERT(resource);
    return _ctx.resource.entries.data[NU_HANDLE_INDEX(resource)].type;
}
nu_uid_t
nu_resource_get_uid (nu_resource_t resource)
{
    NU_ASSERT(resource);
    return _ctx.resource.entries.data[NU_HANDLE_INDEX(resource)].uid;
}

#endif
