#ifndef NU_ECS_DUMP_IMPL_H
#define NU_ECS_DUMP_IMPL_H

#include <nucleus/internal.h>

static void
nu__ecs_print_with_depth (nu_size_t depth, nu_str_t format, ...)
{
    va_list args;
    va_start(args, format);
    NU_STR_BUF(str, 256);
    nu_str_vfmt(str, format, args);
    NU_INFO("%*s%s", depth * 2, "", str);
    va_end(args);
}
static void nu__ecs_dump_attribute(nu_size_t                  depth,
                                   const nu__ecs_attribute_t *a,
                                   const nu_byte_t           *data);
static void
nu__ecs_dump_component (nu_size_t        depth,
                        nu_ecs_id_t      c,
                        nu_size_t        count,
                        const nu_byte_t *data)
{
    const nu__ecs_component_t *comp = _ctx.ecs.components.data + NU_ID_INDEX(c);
    nu__ecs_print_with_depth(
        depth,
        NU_STR(NU_STR_FMT " {"),
        NU_STR_ARGS(nu_str_from_cstr((nu_byte_t *)comp->name)));
    for (nu_size_t i = 0; i < count; ++i)
    {
        for (nu_size_t j = 0; j < _ctx.ecs.attributes.size; ++j)
        {
            const nu__ecs_attribute_t *a = _ctx.ecs.attributes.data + j;
            if (a->component == c)
            {
                const nu_byte_t *p = data + a->offset;
                nu__ecs_dump_attribute(depth + 1, a, data);
            }
        }
    }
    nu__ecs_print_with_depth(depth, NU_STR("}"));
}
static void
nu__ecs_dump_object (nu_size_t        depth,
                     nu_object_type_t type,
                     nu_size_t        count,
                     const nu_byte_t *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_byte_t   *p   = (nu_byte_t *)data + sizeof(nu_object_t) * i;
        nu_object_t *obj = (nu_object_t *)p;
        if (*obj)
        {
            nu_uid_t uid = nu_object_get_uid(obj);
            NU_ASSERT(uid);
            nu__ecs_print_with_depth(depth, NU_STR("%llu"), uid);
        }
        else
        {
            nu__ecs_print_with_depth(depth, NU_STR("null"));
        }
    }
}
static void
nu__ecs_dump_str (nu_size_t        depth,
                  nu_size_t        capacity,
                  nu_size_t        count,
                  const nu_byte_t *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        nu_str_t str = nu_str_from_cstr((nu_byte_t *)data + capacity * i);
        nu__ecs_print_with_depth(
            depth, NU_STR("'" NU_STR_FMT "'"), NU_STR_ARGS(str));
    }
}
static void
nu__ecs_dump_primitive (nu_size_t          depth,
                        nu_ecs_primitive_t primitive,
                        nu_size_t          count,
                        const nu_byte_t   *data)
{
    for (nu_size_t i = 0; i < count; ++i)
    {
        const nu_byte_t *p = data + nu__ecs_primitive_size(primitive) * i;
        switch (primitive)
        {
            case NU_ECS_BYTE:
                nu__ecs_print_with_depth(depth, NU_STR("%d"), *(nu_u8_t *)p);
                break;
            case NU_ECS_U32:
                nu__ecs_print_with_depth(depth, NU_STR("%d"), *(nu_u32_t *)p);
                break;
            case NU_ECS_F32:
                nu__ecs_print_with_depth(depth, NU_STR("%lf"), *(nu_f32_t *)p);
                break;
            case NU_ECS_V3: {
                nu_v3_t *v = (nu_v3_t *)p;
                nu__ecs_print_with_depth(
                    depth, NU_STR("[" NU_V3_FMT "]"), NU_V3_ARGS(v));
            }
            break;
            case NU_ECS_Q4: {
                nu_q4_t *v = (nu_q4_t *)p;
                nu__ecs_print_with_depth(
                    depth, NU_STR("[" NU_Q4_FMT "]"), NU_Q4_ARGS(v));
            }
            break;
            case NU_ECS_M4: {
                nu_m4_t *m = (nu_m4_t *)p;
                nu__ecs_print_with_depth(
                    depth, NU_STR("[" NU_M4_FMT "]"), NU_M4_ARGS(m));
            }
            break;
        }
    }
}
static void
nu__ecs_dump_attribute (nu_size_t                  depth,
                        const nu__ecs_attribute_t *a,
                        const nu_byte_t           *data)
{
    nu__ecs_print_with_depth(
        depth,
        NU_STR(NU_STR_FMT ":"),
        NU_STR_ARGS(nu_str_from_cstr((nu_byte_t *)a->name)));
    const nu_byte_t *p = data + a->offset;
    switch (a->info.kind)
    {
        case NU_ECS_ATTR_PRIMITIVE:
            nu__ecs_dump_primitive(depth + 1, a->info.p, a->count, p);
            break;
        case NU_ECS_ATTR_COMPONENT:
            nu__ecs_dump_component(depth + 1, a->info.c, a->count, p);
            break;
        case NU_ECS_ATTR_OBJECT:
            nu__ecs_dump_object(depth + 1, a->info.o, a->count, p);
            break;
        case NU_ECS_ATTR_STRING:
            nu__ecs_dump_str(depth + 1, a->info.s, a->count, p);
            break;
    }
}
void
nu_ecs_dump_component (nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c)
{
    const nu__ecs_component_t *comp = _ctx.ecs.components.data + c;
    const void                *data = nu_ecs_get(ecs, e, c);
    nu__ecs_dump_component(0, c, 1, data);
}
void
nu_ecs_dump_entity (nu_ecs_t ecs, nu_ecs_id_t e)
{
    const nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    NU_INFO("entity #%d {", e);
    for (nu_size_t c = 0; c < ins->pools.size; ++c)
    {
        nu_ecs_id_t cid = NU_ID_MAKE(c);
        if (nu_ecs_has(ecs, e, cid))
        {
            const void *data = nu_ecs_get(ecs, e, cid);
            nu__ecs_dump_component(1, cid, 1, data);
        }
    }
    NU_INFO("}");
}
void
nu_ecs_dump (nu_ecs_t ecs)
{
    const nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;
    for (nu_size_t i = 0; i < nu_ecs_capacity(ecs); ++i)
    {
        nu_ecs_id_t e = NU_ID_MAKE(i);
        if (nu_ecs_valid(ecs, e))
        {
            nu_ecs_dump_entity(ecs, e);
        }
    }
}

#endif
