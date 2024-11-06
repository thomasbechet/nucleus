#ifndef NU_ECS_SERIA_IMPL_H
#define NU_ECS_SERIA_IMPL_H

#include <nucleus/internal.h>

void
nu_ecs_write (nu_ecs_t ecs, nu_seria_t seria)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    // write ecs header
    nu_u32_t pool_count = ins->pools.size;
    nu_seria_write(seria, NU_SERIA_U32, 1, &pool_count);

    // iterate pools
    for (nu_size_t p = 0; p < ins->pools.size; ++p)
    {
        const nu__ecs_component_pool_t *pool = ins->pools.data + p;
        nu_u32_t component_count = nu__ecs_bitset_count(&pool->bitset);
        if (!component_count)
        {
            continue;
        }
        const nu__ecs_component_t *comp = _ctx.ecs.components.data + p;
        nu_str_t component_name = nu_str_from_cstr((nu_byte_t *)comp->name);

        // write component name
        nu_seria_write(
            seria, NU_SERIA_STR, component_name.size, component_name.data);

        // write component count
        nu_seria_write(seria, NU_SERIA_U32, component_count, &component_count);

        // write component data
        nu_size_t entity_count = nu_ecs_count(ecs);
        for (nu_size_t i = 0; i < entity_count; ++i)
        {
            if (nu__ecs_bitset_isset(&pool->bitset, i))
            {
                // write entity
                nu_ecs_id_t id = i + 1;
                nu_seria_write(seria, NU_SERIA_U32, 1, &id);

                // write data
                void *data = nu_ecs_get(ecs, i + 1, p);
                nu_seria_write(seria, comp->layout, 1, data);
            }
        }
    }
}
void
nu_ecs_read (nu_ecs_t ecs, nu_seria_t seria)
{
    nu_ecs_clear(ecs);

    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    // read ecs header
    nu_u32_t pool_count;
    nu_seria_read(seria, NU_SERIA_U32, 1, &pool_count);

    for (nu_size_t p = 0; p < pool_count; ++p)
    {
        // read component name
        nu_byte_t component_name[NU__ECS_COMPONENT_NAME_LEN];
        nu_seria_read(
            seria, NU_SERIA_STR, NU__ECS_COMPONENT_NAME_LEN, component_name);

        // find component
        nu_ecs_id_t component
            = nu_ecs_find_component(nu_str_from_cstr(component_name));

        // read component count
        nu_u32_t component_count;
        nu_seria_read(seria, NU_SERIA_U32, 1, &component_count);
        NU_ASSERT(component_count > 0);

        // find component layout
        nu_seria_layout_t layout = _ctx.ecs.components.data[component].layout;
        NU_ASSERT(layout);

        // read component data
        for (nu_size_t i = 0; i < component_count; ++i)
        {
            // read entity
            nu_u32_t id;
            nu_seria_read(seria, NU_SERIA_U32, 1, &id);
            NU_ASSERT(id);
            nu_ecs_id_t e = id;

            // create entity if needed
            if (nu_ecs_valid(ecs, e))
            {
                nu_ecs_add_at(ecs, e);
            }

            // set component
            void *data = nu_ecs_set(ecs, e, component);

            // read component
            nu_seria_read(seria, layout, 1, data);
        }
    }
}

#endif
