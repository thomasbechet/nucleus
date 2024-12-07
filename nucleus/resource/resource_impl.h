#ifndef NU_RES_IMPL_H
#define NU_RES_IMPL_H

#include <nucleus/internal.h>

static nu_error_t
nu__resource_init (void)
{
    _ctx.resource.obj_bundle
        = nu_object_type_new(NU_STR("bundle"), sizeof(nu__bundle_t), NU_NULL);
    return NU_ERROR_NONE;
}
static nu_error_t
nu__resource_free (void)
{
    return NU_ERROR_NONE;
}

nu_bundle_t
nu_bundle_new (nu_size_t capacity)
{
    nu__bundle_t *b = nu_object_new(_ctx.resource.obj_bundle);
    NU_VEC_ALLOC(&b->objects, capacity);
    return (nu_bundle_t)b;
}
void
nu_bundle_add (nu_bundle_t bundle, nu_object_t object)
{
    nu__bundle_t *b = (nu__bundle_t *)bundle;
    for (nu_size_t i = 0; i < b->objects.size; ++i)
    {
        if (b->objects.data[i] == object)
        {
            NU_ERROR("adding existing object to bundle");
            return;
        }
    }
    NU_ASSERT(object);
    *(NU_VEC_PUSH(&b->objects)) = object;
}
nu_size_t
nu_bundle_size (nu_bundle_t bundle)
{
    nu__bundle_t *b = (nu__bundle_t *)bundle;
    return b->objects.size;
}
nu_object_t
nu_bundle_get (nu_bundle_t bundle, nu_size_t index)
{
    nu__bundle_t *b = (nu__bundle_t *)bundle;
    NU_ASSERT(index < b->objects.size);
    return b->objects.data[index];
}
nu_bundle_t
nu_bundle_load (nu_seria_t seria)
{
    nu_u32_t count;
    nu_seria_read_u32(seria, 1, &count); // read resource count
    NU_ASSERT(count < 1024);

    nu_bundle_t bundle = nu_bundle_new(count);

    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_uid_t type_uid;
        nu_seria_read_u32(seria, 1, &type_uid); // read type uid
        nu_uid_t uid;
        nu_seria_read_u32(seria, 1, &uid); // read resource uid
        NU_ASSERT(uid);

        nu_object_type_t type = nu_object_type_find(type_uid);
        NU_ASSERT(type);
        nu_object_t obj = nu_seria_load_object(seria, type);
        NU_ASSERT(obj);

        nu_object_set_uid(obj, uid);
        nu_bundle_add(bundle, obj);
    }

    return bundle;
}
void
nu_bundle_save (nu_bundle_t bundle, nu_seria_t seria)
{
    nu_u32_t count = nu_bundle_size(bundle);
    nu_seria_write_u32(seria, 1, &count);

    for (nu_size_t i = 0; i < nu_bundle_size(bundle); ++i)
    {
        nu_object_t obj      = nu_bundle_get(bundle, i);
        nu_uid_t    type_uid = nu_object_get_uid(nu_object_get_type(obj));
        NU_ASSERT(type_uid);
        nu_seria_write_u32(seria, 1, &type_uid);

        nu_uid_t uid = nu_object_get_uid(obj);
        NU_ASSERT(uid);
        nu_seria_write_u32(seria, 1, &uid);
    }
}

#endif
