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
        nu_resource_unload(res->uid);
    }
    NU_VEC_FREE(&_ctx.resource.entries);
    NU_VEC_FREE(&_ctx.resource.types);
    return NU_ERROR_NONE;
}

static nu__resource_type_t *
nu__resource_type_find (nu_uid_t uid)
{
    for (nu_size_t i = 0; i < _ctx.resource.types.size; ++i)
    {
        nu__resource_type_t *t = _ctx.resource.types.data + i;
        if (t->uid == uid)
        {
            return t;
        }
    }
    return NU_NULL;
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
void
nu_resource_register (nu_uid_t             uid,
                      nu_resource_load_t   load,
                      nu_resource_unload_t unload,
                      nu_resource_save_t   save)
{
    if (nu__resource_type_find(uid))
    {
        NU_ERROR("resource type %d already exists", uid);
        return;
    }

    nu__resource_type_t *t = NU_VEC_PUSH(&_ctx.resource.types);
    t->uid                 = uid;
    t->load                = load;
    t->unload              = unload;
    t->save                = save;
}

void
nu_resource_insert (nu_uid_t type, nu_uid_t uid, void *data)
{
    NU_ASSERT(type);
    NU_ASSERT(data);

    const nu__resource_type_t *t = nu__resource_type_find(type);
    NU_ASSERT(t);
    if (nu__resource_find(uid, NU_NULL))
    {
        NU_ERROR("resource already exists %d", uid);
        return;
    }

    nu__resource_entry_t *res = NU_VEC_PUSH(&_ctx.resource.entries);
    res->type                 = type;
    res->uid                  = uid;
    res->bundle               = _ctx.resource.active_bundle;
    res->data                 = data;
    res->refcount             = 0;
}
static void
nu__resource_remove_index (nu_size_t index)
{
    nu__resource_entry_t      *res = _ctx.resource.entries.data + index;
    const nu__resource_type_t *t
        = _ctx.resource.types.data + NU_HANDLE_INDEX(res->type);

    NU_ASSERT(res->refcount == 0);

    // free resource
    if (res->data && t->unload)
    {
        t->unload(res->data);
    }

    res->data = NU_NULL;

    NU_VEC_SWAP_REMOVE(&_ctx.resource.entries, index);
}
void
nu_resource_set_default_bundle (nu_uid_t uid)
{
    _ctx.resource.active_bundle = uid;
}
void
nu_resource_set_bundle (nu_uid_t uid, nu_uid_t bundle)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    res->bundle = bundle;
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
void *
nu_resource_data (nu_uid_t type, nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    return res->type == type ? res->data : NU_NULL;
}
nu_uid_t
nu_resource_type (nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    return res->type;
}
nu_uid_t
nu_resource_load (nu_seria_t seria)
{
    nu_uid_t type = nu_seria_read_u32(seria); // read resource type
    NU_ASSERT(type);
    nu_uid_t uid = nu_seria_read_u32(seria); // read resource uid
    NU_ASSERT(uid);

    const nu__resource_type_t *t = nu__resource_type_find(type);
    NU_ASSERT(t);

    nu_resource_insert(type, uid, NU_NULL); // check duplicated

    nu__resource_entry_t *res = NU_VEC_LAST(&_ctx.resource.entries);
    if (t->load)
    {
        res->data = t->load(seria);
    }
    return uid;
}
nu_uid_t
nu_resource_load_bundle (nu_seria_t seria)
{
    nu_uid_t  bundle    = nu_seria_read_u32(seria); // read bundle uid
    nu_size_t res_count = nu_seria_read_u32(seria); // read resource count

    nu_uid_t prev_bundle = _ctx.resource.active_bundle;
    nu_resource_set_default_bundle(bundle);
    for (nu_size_t i = 0; i < res_count; ++i)
    {
        nu_resource_load(seria); // read resource
    }
    nu_resource_set_default_bundle(prev_bundle);
    return bundle;
}
void
nu_resource_unload (nu_uid_t uid)
{
    NU_ASSERT(uid);
    nu_size_t index;
    if (nu__resource_find(uid, &index))
    {
        nu__resource_remove_index(index);
    }
}
void
nu_resource_unload_bundle (nu_uid_t uid)
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
nu_resource_save (nu_uid_t uid, nu_seria_t seria)
{
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    nu__resource_type_t *t = nu__resource_type_find(res->type);
    nu_seria_write_u32(seria, res->type); // write resource type
    nu_seria_write_u32(seria, uid);       // write resource uid
    if (t->save)
    {
        t->save(res->data, seria);
    }
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
            const nu__resource_type_t *t = nu__resource_type_find(res->type);
            NU_ASSERT(t);
            nu_resource_save(res->uid, seria); // write resource
        }
    }
}

#endif
