#ifndef NU_ECS_SERIA_IMPL_H
#define NU_ECS_SERIA_IMPL_H

#include <nucleus/internal.h>

void
nu_ecs_save (nu_ecs_t ecs, nu_seria_t seria)
{
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    // write ecs header
    nu_u32_t capacity = nu_ecs_capacity(ecs);
    nu_u32_t count    = ins->pools.size;
    nu_seria_write_u32(seria, 1, &capacity);
    nu_seria_write_u32(seria, 1, &count);

    // iterate pools
    for (nu_size_t c = 0; c < ins->pools.size; ++c)
    {
        nu_ecs_id_t                     cid  = NU_ID_MAKE(c);
        const nu__ecs_component_pool_t *pool = ins->pools.data + c;
        nu_u32_t component_count = nu__ecs_bitset_count(&pool->bitset);
        if (!component_count)
        {
            continue;
        }
        const nu__ecs_component_t *comp = _ctx.ecs.components.data + c;
        nu_str_t component_name = nu_str_from_cstr((nu_byte_t *)comp->name);

        // write component name
        nu_seria_write_str(seria, component_name);

        // write component count
        nu_seria_write_u32(seria, 1, &component_count);

        // write component data
        nu_size_t entity_count = nu_ecs_count(ecs);
        for (nu_size_t i = 0; i < entity_count; ++i)
        {
            if (nu__ecs_bitset_isset(&pool->bitset, i))
            {
                // write entity
                nu_ecs_id_t id = NU_ID_MAKE(i);
                nu_seria_write_u32(seria, 1, &id);

                // write data
                nu_ecs_write_component(ecs, id, cid, seria);
            }
        }
    }
}
nu_ecs_t
nu_ecs_load (nu_seria_t seria)
{
    // read ecs header
    nu_u32_t capacity;
    nu_seria_read_u32(seria, 1, &capacity);
    NU_ASSERT(capacity < 1024);
    nu_u32_t pool_count;
    nu_seria_read_u32(seria, 1, &pool_count);
    NU_ASSERT(pool_count < 1024);

    nu_ecs_t            ecs = nu_ecs_new(capacity);
    nu__ecs_instance_t *ins = (nu__ecs_instance_t *)ecs;

    for (nu_size_t p = 0; p < pool_count; ++p)
    {
        // read component name
        nu_byte_t component_name_buf[NU__ECS_COMPONENT_NAME_LEN];
        nu_str_t  component_name = nu_seria_read_str(
            seria, NU__ECS_COMPONENT_NAME_LEN, component_name_buf);

        // find component
        nu_ecs_id_t cid = nu_ecs_find_component(component_name);
        NU_ASSERT(cid);

        // read component count
        nu_u32_t component_count;
        nu_seria_read_u32(seria, 1, &component_count);
        NU_ASSERT(component_count > 0);

        // read component data
        for (nu_size_t i = 0; i < component_count; ++i)
        {
            // read entity
            nu_ecs_id_t e;
            nu_seria_read_u32(seria, 1, &e);
            NU_ASSERT(e);

            // create entity if needed
            if (!nu_ecs_valid(ecs, e))
            {
                nu_ecs_add_at(ecs, e);
            }

            // set component
            void *data = nu_ecs_set(ecs, e, cid);

            // read component
            nu_ecs_read_component(ecs, e, cid, seria);
        }
    }

    return ecs;
}
static void
nu__ecs_read_component (nu_seria_t seria, nu_ecs_id_t c, nu_byte_t *data)
{
    for (nu_size_t i = 0; i < _ctx.ecs.attributes.size; ++i)
    {
        const nu__ecs_attribute_t *attr = _ctx.ecs.attributes.data + i;
        if (attr->component == c)
        {
            nu_byte_t *p = data + attr->offset;
            switch (attr->info.kind)
            {
                case NU_ECS_ATTR_PRIMITIVE:
                    switch (attr->info.p)
                    {
                        case NU_ECS_BYTE:
                            nu_seria_read_byte(seria, attr->count, p);
                            break;
                        case NU_ECS_U32:
                            nu_seria_read_u32(
                                seria, attr->count, (nu_u32_t *)p);
                            break;
                        case NU_ECS_F32:
                            nu_seria_read_f32(
                                seria, attr->count, (nu_f32_t *)p);
                            break;
                        case NU_ECS_V3:
                            nu_seria_read_v3(seria, attr->count, (nu_v3_t *)p);
                            break;
                        case NU_ECS_Q4:
                            nu_seria_read_q4(seria, attr->count, (nu_q4_t *)p);
                            break;
                        case NU_ECS_M4:
                            nu_seria_read_m4(seria, attr->count, (nu_m4_t *)p);
                            break;
                    }
                    break;
                case NU_ECS_ATTR_COMPONENT:
                    for (nu_size_t j = 0; j < attr->count; ++j)
                    {
                        nu_size_t size = nu_ecs_component_size(attr->info.c);
                        nu__ecs_read_component(
                            seria, attr->info.c, p + size * j);
                    }
                    break;
                case NU_ECS_ATTR_OBJECT:
                    for (nu_size_t j = 0; j < attr->count; ++j)
                    {
                        nu_object_t obj = ((nu_object_t)p) + j;
                        nu_seria_read_object(seria, obj);
                    }
                    break;
                case NU_ECS_ATTR_STRING:
                    for (nu_size_t j = 0; j < attr->count; ++j)
                    {
                        nu_seria_read_str(
                            seria, attr->info.s, p + j * attr->info.s);
                    }
                    break;
            }
        }
    }
}
void
nu_ecs_read_component (nu_ecs_t    ecs,
                       nu_ecs_id_t e,
                       nu_ecs_id_t c,
                       nu_seria_t  seria)
{
    nu_byte_t *p = nu_ecs_get(ecs, e, c);
    NU_ASSERT(p);
    nu__ecs_read_component(seria, c, p);
}
static void
nu__ecs_write_component (nu_seria_t seria, nu_ecs_id_t c, const nu_byte_t *data)
{
    for (nu_size_t i = 0; i < _ctx.ecs.attributes.size; ++i)
    {
        const nu__ecs_attribute_t *attr = _ctx.ecs.attributes.data + i;
        if (attr->component == c)
        {
            const nu_byte_t *p = data + attr->offset;
            switch (attr->info.kind)
            {
                case NU_ECS_ATTR_PRIMITIVE:
                    switch (attr->info.p)
                    {
                        case NU_ECS_BYTE:
                            nu_seria_write_byte(seria, attr->count, p);
                            break;
                        case NU_ECS_U32:
                            nu_seria_write_u32(
                                seria, attr->count, (const nu_u32_t *)p);
                            break;
                        case NU_ECS_F32:
                            nu_seria_write_f32(
                                seria, attr->count, (const nu_f32_t *)p);
                            break;
                        case NU_ECS_V3:
                            nu_seria_write_v3(
                                seria, attr->count, (const nu_v3_t *)p);
                            break;
                        case NU_ECS_Q4:
                            nu_seria_write_q4(
                                seria, attr->count, (const nu_q4_t *)p);
                            break;
                        case NU_ECS_M4:
                            nu_seria_write_m4(
                                seria, attr->count, (const nu_m4_t *)p);
                            break;
                    }
                    break;
                case NU_ECS_ATTR_COMPONENT:
                    for (nu_size_t j = 0; j < attr->count; ++j)
                    {
                        nu_size_t size = nu_ecs_component_size(attr->info.c);
                        nu__ecs_write_component(
                            seria, attr->info.c, p + size * j);
                    }
                    break;
                case NU_ECS_ATTR_OBJECT:
                    for (nu_size_t j = 0; j < attr->count; ++j)
                    {
                        nu_object_t obj = ((nu_object_t)p) + j;
                        nu_seria_write_object(seria, obj);
                    }
                    break;
                case NU_ECS_ATTR_STRING:
                    for (nu_size_t j = 0; j < attr->count; ++j)
                    {
                        nu_str_t str = nu_str_from_cstr((nu_byte_t *)p
                                                        + j * attr->info.s);
                        nu_seria_write_str(seria, str);
                    }
                    break;
            }
        }
    }
}
void
nu_ecs_write_component (nu_ecs_t    ecs,
                        nu_ecs_id_t e,
                        nu_ecs_id_t c,
                        nu_seria_t  seria)
{
    const nu_byte_t *p = nu_ecs_get(ecs, e, c);
    NU_ASSERT(p);
    nu__ecs_write_component(seria, c, p);
}

#endif
