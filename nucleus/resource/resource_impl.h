#ifndef NU_RESOURCE_IMPL_H
#define NU_RESOURCE_IMPL_H

#include <nucleus/internal.h>

static nu_error_t
nu__resource_init (void)
{
    NU_VEC_INIT(10, &_ctx.resource.types);
    NU_VEC_INIT(10, &_ctx.resource.entries);
    _ctx.resource.active_bundle = NU_NULL;
    return NU_ERROR_NONE;
}
static nu_error_t
nu__resource_free (void)
{
    while (_ctx.resource.entries.size)
    {
        const nu__resource_entry_t *res
            = _ctx.resource.entries.data + _ctx.resource.entries.size - 1;
        nu_resource_remove(res->uid);
    }
    NU_VEC_FREE(&_ctx.resource.entries);
    NU_VEC_FREE(&_ctx.resource.types);
    return NU_ERROR_NONE;
}

nu_resource_t
nu_resource_register (nu_uid_t              uid,
                      nu_resource_added_t   added,
                      nu_resource_removed_t removed,
                      nu_resource_load_t    load,
                      nu_resource_save_t    save)
{
    if (nu_resource_find_type(uid))
    {
        NU_ERROR("resource type %d already exists", uid);
        return NU_NULL;
    }

    nu__resource_type_t *t = NU_VEC_PUSH(&_ctx.resource.types);
    t->uid                 = uid;
    t->added               = added;
    t->removed             = removed;
    t->load                = load;
    t->save                = save;
    return NU_HANDLE_MAKE(nu_resource_t, _ctx.resource.types.size - 1);
}
nu_resource_t
nu_resource_find_type (nu_uid_t uid)
{
    for (nu_size_t i = 0; i < _ctx.resource.types.size; ++i)
    {
        const nu__resource_type_t *t = _ctx.resource.types.data + i;
        if (t->uid == uid)
        {
            return NU_HANDLE_MAKE(nu_resource_t, i);
        }
    }
    return NU_NULL;
}

void
nu_resource_add (nu_resource_t type, nu_uid_t uid, void *data)
{
    NU_ASSERT(type);
    NU_ASSERT(data);

    const nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(type);

    nu_size_t             index;
    nu__resource_entry_t *res = NU_VEC_PUSH(&_ctx.resource.entries);
    res->uid                  = uid;
    res->bundle               = _ctx.resource.active_bundle;
    res->type                 = type;
    res->data                 = data;
    res->refcount             = 0;

    if (t->added)
    {
        t->added(res->data);
    }
}
static nu__resource_entry_t *
nu__resource_find (nu_uid_t uid, nu_size_t *index)
{
    for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    {
        nu__resource_entry_t *e = _ctx.resource.entries.data + i;
        if (e->uid == uid)
        {
            if (index)
            {
                *index = i;
            }
            return e;
        }
    }
    return NU_NULL;
}
static void
nu__resource_remove_index (nu_size_t index)
{
    nu__resource_entry_t      *res = _ctx.resource.entries.data + index;
    const nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(res->type);

    NU_ASSERT(res->refcount == 0);

    // free resource
    if (res->data && t->removed)
    {
        t->removed(res->data);
    }

    res->data = NU_NULL;

    NU_VEC_SWAP_REMOVE(&_ctx.resource.entries, index);
}
void
nu_resource_remove (nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu_size_t index;
    if (nu__resource_find(uid, &index))
    {
        nu__resource_remove_index(index);
    }
}
void
nu_resource_remove_bundle (nu_uid_t uid)
{
    // remove in reverse to avoid vector swap remove
    for (nu_size_t i = _ctx.resource.entries.size; i > 0; --i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + (i - 1);
        if (res->bundle == uid)
        {
            nu__resource_remove_index(i - 1);
        }
    }
}
void
nu_resource_bind_bundle (nu_uid_t uid)
{
    _ctx.resource.active_bundle = uid;
}
void *
nu_resource_data (nu_resource_t type, nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    if (res->type != type)
    {
        return NU_NULL;
    }
    return res->data;
}
void
nu_resource_acquire (nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    ++res->refcount;
}
void
nu_resource_release (nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res->refcount);
    --res->refcount;
}
nu_resource_t
nu_resource_type (nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    return res->type;
}
nu_uid_t
nu_resource_load (nu_resource_t type, nu_seria_t seria)
{
    NU_ASSERT(type);

    nu_uid_t uid = nu_seria_read_u32(seria); // read resource uid
    NU_ASSERT(uid);

    const nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(type);
    nu__resource_entry_t *res = NU_VEC_PUSH(&_ctx.resource.entries);
    res->uid                  = uid;
    res->type                 = type;
    res->refcount             = 0;
    if (t->load)
    {
        res->data = t->load(seria);
    }
    else
    {
        res->data = NU_NULL;
    }
    return uid;
}
void
nu_resource_save (nu_uid_t uid, nu_seria_t seria)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(res->type);
    nu_seria_write_u32(seria, uid); // write resource uid
    if (t->save)
    {
        t->save(res->data, seria);
    }
}
nu_uid_t
nu_resource_load_bundle (nu_seria_t seria)
{
    nu_uid_t  bundle_uid = nu_seria_read_u32(seria); // read bundle uid
    nu_size_t res_count  = nu_seria_read_u32(seria); // read resource count
    for (nu_size_t i = 0; i < res_count; ++i)
    {
        nu_uid_t type_uid  = nu_seria_read_u32(seria); // read resource type uid
        nu_resource_t type = nu_resource_find_type(type_uid);
        if (!type)
        {
            NU_ERROR("resource type %llu not found", type_uid);
            return NU_NULL;
        }
        nu_resource_load(type, seria); // read resource
    }
    return bundle_uid;
}
void
nu_resource_save_bundle (nu_uid_t uid, nu_seria_t seria)
{
    nu_seria_write_u32(seria, uid); // write bundle uid
    nu_size_t res_count = 0;
    for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
        if (res->bundle == uid)
        {
            ++res_count;
        }
    }
    nu_seria_write_u32(seria, res_count); // write resource count
    for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
        if (res->bundle == uid)
        {
            const nu__resource_type_t *t
                = _ctx.resource.types.data + NU_HANDLE_INDEX(res->type);
            nu_seria_write_u32(seria, t->uid); // write resource type uid
            nu_resource_save(res->uid, seria); // write resource
        }
    }
}

#endif
