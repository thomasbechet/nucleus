#ifndef NU_ECS_SERIA_IMPL_H
#define NU_ECS_SERIA_IMPL_H

#include <nucleus/internal.h>

#ifdef NU_BUILD_ECS_SERIA
nu_ecs_id_t
nu_ecs_register_seria (nu_ecs_t ecs, nu_seria_layout_t layout)
{
    const nu__seria_layout_t *t
        = _ctx.seria.layouts.data + NU_HANDLE_INDEX(layout);
    nu__ecs_instance_t *ins  = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);
    nu_ecs_id_t         id   = nu_ecs_register(ecs, t->size);
    nu__ecs_comp_t     *comp = ins->components.data + id;
    comp->layout             = layout;
    return id;
}
void
nu_ecs_write (nu_ecs_t ecs, nu_seria_t seria)
{
    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    // build dtos
    for (nu_size_t c = 0; c < ins->components.size; ++c)
    {
        const nu__ecs_comp_t *comp = ins->components.data + c;
        if (!comp->layout)
        {
            continue;
        }

        nu_u32_t  component_count = nu__ecs_bitset_count(&comp->bitset);
        nu_str_t  component_name  = nu_seria_name(comp->layout);
        nu_size_t entity_count    = nu_ecs_count(ecs);

        // write component header
        nu_seria_write(
            seria, NU_SERIA_STR, component_name.size, component_name.data);
        nu_seria_write(seria, NU_SERIA_U32, component_count, &component_count);
        for (nu_size_t i = 0; i < entity_count; ++i)
        {
            if (nu__ecs_bitset_isset(&comp->bitset, i))
            {
                void *data = nu_ecs_get(ecs, i + 1, c);
                nu_seria_write(seria, 1, data);
            }
        }

        // write entity indices
        nu_seria_buffer_t entity_buffer
            = nu_seria_write_begin(seria, NU_SERIA_U32, component_count);
        for (nu_size_t i = 0; i < entity_count; ++i)
        {
            if (nu__ecs_bitset_isset(&comp->bitset, i))
            {
                nu_ecs_id_t id = i + 1;
                nu_seria_write(seria, 1, &id);
            }
        }

        // build dto
        nu__ecs_comp_dto_t *dto = dtos + dto_count;
        nu_str_to_cstr(component_name, dto->name, 64);
        dto->entities = entity_buffer;
        dto->data     = data_buffer;
        ++dto_count;
    }

    // write dtos
    nu_seria_buffer_t buffer
        = nu_seria_write_begin(seria, _ctx.ecs.component_type, dto_count);
    nu_seria_write(seria, dto_count, dtos);

    return buffer;
}
void
nu_ecs_read (nu_ecs_t ecs, nu_seria_t seria, nu_seria_buffer_t buffer)
{
    nu_ecs_clear(ecs);

    nu__ecs_instance_t *ins = _ctx.ecs.instances.data + NU_HANDLE_INDEX(ecs);

    NU_VEC(nu_ecs_id_t) entities;
    NU_VEC_INIT(64, &entities);

    nu__ecs_comp_dto_t dtos[256];
    nu_size_t          dto_count
        = nu_seria_read_begin(seria, _ctx.ecs.component_type, buffer);
    NU_ASSERT(dto_count <= NU_ARRAY_SIZE(dtos));
    nu_seria_read(seria, dto_count, dtos);

    for (nu_size_t i = 0; i < dto_count; ++i)
    {
        const nu__ecs_comp_dto_t *dto = dtos + i;
        NU_ASSERT(dto->data && dto->entities);

        // find registered component
        const nu__ecs_comp_t *comp = NU_NULL;
        nu_ecs_id_t           comp_id;
        nu_str_t dto_name = nu_str_from_cstr((nu_byte_t *)dto->name);
        for (nu_size_t c = 0; c < ins->components.size; ++c)
        {
            const nu__ecs_comp_t *component = ins->components.data + c;
            if (component->layout)
            {
                if (nu_str_eq(dto_name, nu_seria_name(component->layout)))
                {
                    comp    = component;
                    comp_id = c;
                    break;
                }
            }
        }
        if (!comp)
        {
            NU_ERROR("ECS component not found" NU_STR_FMT " , skipping...",
                     NU_STR_ARGS(dto_name));
            continue;
        }

        // create entities (if missing)
        NU_VEC_READ(&entities, seria, NU_SERIA_U32, dto->entities);
        for (nu_size_t e = 0; e < entities.size; ++e)
        {
            nu_ecs_id_t id = entities.data[e];
            nu_ecs_add_at(ecs, id);
        }

        // create component
        nu_size_t component_count
            = nu_seria_read_begin(seria, comp->layout, dto->data);
        NU_ASSERT(component_count == entities.size);
        for (nu_size_t c = 0; c < component_count; ++c)
        {
            nu_ecs_set(ecs, entities.data[c], comp_id);
            nu_seria_read(seria, 1, nu_ecs_get(ecs, entities.data[c], comp_id));
        }
    }

    NU_VEC_FREE(&entities);
}
#endif

#endif
