#ifndef NU_RESOURCE_IMPL_H
#define NU_RESOURCE_IMPL_H

#include <nucleus/internal.h>

static void *
nu__resource_handler (nu_resource_action_t action,
                      nu_uid_t             type,
                      void                *data,
                      nu_seria_t           seria)
{
    if (type == NU_RESOURCE_IMAGE)
    {
        switch (action)
        {
            case NU_RESOURCE_CREATE:
                break;
            case NU_RESOURCE_DELETE:
                nu_image_delete(data);
                break;
            case NU_RESOURCE_LOAD:
                return nu_image_load(seria);
            case NU_RESOURCE_SAVE:
                nu_image_save(data, seria);
                break;
        }
    }
    else if (type == NU_RESOURCE_TEXTURE)
    {
        switch (action)
        {
            case NU_RESOURCE_CREATE: {
                nu__texture_t *tex
                    = _ctx.graphics.textures.data + NU_HANDLE_INDEX(data);
                NU_ASSERT(tex->image_texture);
            }
            break;
            case NU_RESOURCE_DELETE:
                nu_texture_delete(data);
                break;
            case NU_RESOURCE_LOAD: {
                nu_texture_type_t type = NU_TEXTURE_COLORMAP;
                if (nu_seria_read_u32(seria))
                {
                    type = NU_TEXTURE_CUBEMAP;
                }
                nu_image_t image = nu_image_load(seria);
                NU_ASSERT(image);
                nu_texture_t texture
                    = nu_texture_create_from_image(type, image);
                nu__texture_t *tex
                    = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
                tex->image_texture = image;
                return texture;
            }
            case NU_RESOURCE_SAVE: {
                nu_texture_t   texture = data;
                nu__texture_t *tex
                    = _ctx.graphics.textures.data + NU_HANDLE_INDEX(texture);
                nu_seria_write_u32(seria,
                                   tex->type == NU_TEXTURE_CUBEMAP ? 1 : 0);
                NU_ASSERT(tex->image_texture);
                nu_image_save(tex->image_texture, seria);
            }
            break;
        }
    }
    else if (type == NU_RESOURCE_MODEL)
    {
        switch (action)
        {
            case NU_RESOURCE_CREATE:
                break;
            case NU_RESOURCE_DELETE:
                nu_model_delete(data);
                break;
            case NU_RESOURCE_LOAD:
                break;
            case NU_RESOURCE_SAVE:
                break;
        }
    }
    else if (type == NU_RESOURCE_ECS)
    {
        switch (action)
        {
            case NU_RESOURCE_CREATE:
                break;
            case NU_RESOURCE_DELETE:
                nu_ecs_delete(data);
                break;
            case NU_RESOURCE_LOAD:
                return nu_ecs_load(seria);
            case NU_RESOURCE_SAVE:
                nu_ecs_save(data, seria);
                break;
        }
    }
    else if (type == NU_RESOURCE_INPUT)
    {
        switch (action)
        {
            case NU_RESOURCE_CREATE:
                break;
            case NU_RESOURCE_DELETE:
                break;
            case NU_RESOURCE_LOAD:
                return nu_input_create();
            case NU_RESOURCE_SAVE:
                break;
        }
    }
    return NU_NULL;
}

#define NU__REGISTER(type, name) \
    nu_resource_register(type, NU_STR(#name), nu__resource_handler)

static nu_error_t
nu__resource_init (void)
{
    NU_VEC_INIT(10, &_ctx.resource.types);
    NU_VEC_INIT(10, &_ctx.resource.entries);

    // register base types
    NU__REGISTER(NU_RESOURCE_IMAGE, image);
    NU__REGISTER(NU_RESOURCE_TEXTURE, texture);
    NU__REGISTER(NU_RESOURCE_MODEL, model);
    NU__REGISTER(NU_RESOURCE_ECS, ecs);
    NU__REGISTER(NU_RESOURCE_INPUT, input);

    return NU_ERROR_NONE;
}
static nu_error_t
nu__resource_free (void)
{
    while (_ctx.resource.entries.size)
    {
        const nu__resource_entry_t *res
            = _ctx.resource.entries.data + _ctx.resource.entries.size - 1;
        nu_resource_delete(res->uid);
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
nu_resource_register (nu_uid_t              uid,
                      nu_str_t              name,
                      nu_resource_handler_t handler)
{
    if (nu__resource_type_find(uid))
    {
        NU_ERROR("resource type " NU_STR_FMT " already exists",
                 NU_STR_ARGS(name));
        return;
    }

    nu__resource_type_t *t = NU_VEC_PUSH(&_ctx.resource.types);
    t->uid                 = uid;
    t->name                = name;
    t->handler             = handler;
}

void
nu_resource_create (nu_uid_t type, nu_uid_t group, nu_uid_t uid, void *data)
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
    res->group                = group;
    res->data                 = data;

    t->handler(NU_RESOURCE_CREATE, type, res->data, NU_NULL);
}
static void
nu__resource_remove_index (nu_size_t index)
{
    nu__resource_entry_t      *res = _ctx.resource.entries.data + index;
    const nu__resource_type_t *t   = nu__resource_type_find(res->type);
    NU_ASSERT(t);

    // delete resource
    t->handler(NU_RESOURCE_DELETE, res->type, res->data, NU_NULL);

    NU_VEC_SWAP_REMOVE(&_ctx.resource.entries, index);
}
void
nu_resource_delete (nu_uid_t uid)
{
    nu_size_t index;
    if (nu__resource_find(uid, &index))
    {
        nu__resource_remove_index(index);
    }
}
void *
nu_resource (nu_uid_t type, nu_uid_t uid)
{
    NU_ASSERT(type);
    NU_ASSERT(uid);
    nu__resource_entry_t *res = nu__resource_find(uid, NU_NULL);
    NU_ASSERT(res);
    return res->type == type ? res->data : NU_NULL;
}

nu_uid_t
nu_resource_load_group (nu_seria_t seria)
{
    nu_uid_t  group     = nu_seria_read_u32(seria); // read group uid
    nu_size_t res_count = nu_seria_read_u32(seria); // read resource count

    for (nu_size_t i = 0; i < res_count; ++i)
    {
        nu_uid_t type_uid = nu_seria_read_u32(seria); // read type uid
        nu_uid_t uid      = nu_seria_read_u32(seria); // read resource uid
        NU_ASSERT(uid);

        const nu__resource_type_t *t = nu__resource_type_find(uid);
        NU_ASSERT(t);

        void *data = t->handler(NU_RESOURCE_LOAD, type_uid, NU_NULL, seria);
        NU_ASSERT(data);

        nu_resource_create(type_uid, group, uid, data);
    }
    return group;
}
void
nu_resource_save_group (nu_uid_t uid, nu_seria_t seria)
{
    nu_seria_write_u32(seria, uid); // write bundle uid
    nu_size_t res_count = 0;
    for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
        if (res->group == uid)
        {
            ++res_count;
        }
    }
    nu_seria_write_u32(seria, res_count); // write resource count
    for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
        if (res->group == uid)
        {
            const nu__resource_type_t *t = nu__resource_type_find(res->type);
            NU_ASSERT(t);

            nu_seria_write_u32(seria, t->uid); // write resource type
            nu_seria_write_u32(seria, uid);    // write resource uid
            t->handler(NU_RESOURCE_SAVE, res->type, res->data, seria);
        }
    }
}
void
nu_resource_delete_group (nu_uid_t uid)
{
    // remove in reverse to avoid vector swap remove
    for (nu_size_t i = _ctx.resource.entries.size; i > 0; --i)
    {
        const nu__resource_entry_t *res = _ctx.resource.entries.data + (i - 1);
        if (res->group == uid)
        {
            nu__resource_remove_index(i - 1);
        }
    }
}

#endif
