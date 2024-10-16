#ifndef NU_ECS_API_H
#define NU_ECS_API_H

#include <nucleus/core/api.h>

NU_DEFINE_HANDLE(nu_ecs_t);

typedef nu_u32_t nu_ecs_id_t;

typedef void (*nu_ecs_system_callback_t)(nu_ecs_t           ecs,
                                         const nu_ecs_id_t *entities,
                                         nu_size_t          count);

NU_API nu_ecs_t nu_ecs_create(void);
NU_API void     nu_ecs_delete(nu_ecs_t ecs);

NU_API nu_ecs_id_t nu_ecs_add(nu_ecs_t ecs);
NU_API void        nu_ecs_remove(nu_ecs_t ecs, nu_ecs_id_t e);

NU_API nu_ecs_id_t nu_ecs_reg_component(nu_ecs_t ecs, nu_size_t size);
NU_API nu_ecs_id_t nu_ecs_reg_component_with(nu_ecs_t    ecs,
                                             nu_ecs_id_t id,
                                             nu_size_t   size);
NU_API void       *nu_ecs_set(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void        nu_ecs_unset(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void       *nu_ecs_get(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API nu_bool_t   nu_ecs_has(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);

NU_API nu_ecs_id_t nu_ecs_reg_system(nu_ecs_t                 ecs,
                                     nu_ecs_system_callback_t callback);
NU_API nu_ecs_id_t nu_ecs_reg_system_with(nu_ecs_t                 ecs,
                                          nu_ecs_id_t              id,
                                          nu_ecs_system_callback_t callback);
NU_API void        nu_ecs_requires(nu_ecs_t ecs, nu_ecs_id_t s, nu_ecs_id_t c);
NU_API void        nu_ecs_excludes(nu_ecs_t ecs, nu_ecs_id_t s, nu_ecs_id_t c);

#endif
