#ifndef NU_RES_IMPL_H
#define NU_RES_IMPL_H

#include <nucleus/internal.h>

static nu_error_t
nu__resource_init (void)
{
    return NU_ERROR_NONE;
}
static nu_error_t
nu__resource_free (void)
{
    return NU_ERROR_NONE;
}
nu_uid_t
nu_resource_load (nu_seria_t seria)
{
    nu_uid_t group
        = nu_seria_read_1u32(seria, NU_STR("group")); // read group uid
    nu_size_t res_count
        = nu_seria_read_1u32(seria, NU_STR("count")); // read resource count

    for (nu_size_t i = 0; i < res_count; ++i)
    {
        nu_uid_t type_uid
            = nu_seria_read_1u32(seria, NU_STR("type_uid")); // read type uid
        nu_uid_t uid
            = nu_seria_read_1u32(seria, NU_STR("uid")); // read resource uid
        NU_ASSERT(uid);

        nu_object_type_t type = nu_object_type_find(type_uid);
        NU_ASSERT(type);

        nu_object_t obj = nu_object_load(type, seria);
        NU_ASSERT(obj);

        nu_object_set_uid(obj, uid);
    }
    return group;
}
void
nu_resource_save (nu_uid_t uid, nu_seria_t seria)
{
    // nu_seria_write_1u32(seria, NU_STR("group"), uid); // write group uid
    // nu_size_t res_count = 0;
    // for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    // {
    //     const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
    //     if (res->group == uid)
    //     {
    //         ++res_count;
    //     }
    // }
    // nu_seria_write_1u32(
    //     seria, NU_STR("count"), res_count); // write resource count
    // for (nu_size_t i = 0; i < _ctx.resource.entries.size; ++i)
    // {
    //     const nu__resource_entry_t *res = _ctx.resource.entries.data + i;
    //     if (res->group == uid)
    //     {
    //         const nu__resource_type_t *t = nu__resource_type_find(res->type);
    //         NU_ASSERT(t);
    //
    //         nu_seria_write_1u32(
    //             seria, NU_STR("type_uid"), t->uid); // write resource type
    //         nu_seria_write_1u32(
    //             seria, NU_STR("uid"), uid); // write resource uid
    //         t->handler(NU_RES_SAVE, res->type, res->handle, seria);
    //     }
    // }
}

#endif
