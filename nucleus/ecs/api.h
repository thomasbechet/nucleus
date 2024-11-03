#ifndef NU_ECS_API_H
#define NU_ECS_API_H

#include <nucleus/seria/api.h>

NU_DEFINE_HANDLE(nu_ecs_t);

#define NU_ECS_ITER(ecs, iter, e)                           \
    for (nu_ecs_id_t(e) = nu_ecs_begin((ecs), (iter)); (e); \
         (e)            = nu_ecs_next((ecs), (iter)))

typedef nu_u32_t nu_ecs_id_t;

NU_API nu_ecs_t nu_ecs_create(void);
NU_API void     nu_ecs_delete(nu_ecs_t ecs);

NU_API nu_ecs_id_t nu_ecs_add(nu_ecs_t ecs);
NU_API void        nu_ecs_add_at(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API void        nu_ecs_remove(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API nu_bool_t   nu_ecs_valid(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API nu_size_t   nu_ecs_count(nu_ecs_t ecs);
NU_API nu_size_t   nu_ecs_capacity(nu_ecs_t ecs);

NU_API nu_ecs_id_t nu_ecs_register(nu_ecs_t ecs, nu_size_t size);
NU_API void       *nu_ecs_set(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void        nu_ecs_unset(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void       *nu_ecs_get(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API nu_bool_t   nu_ecs_has(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);

NU_API nu_ecs_id_t nu_ecs_iter(nu_ecs_t ecs);
NU_API void nu_ecs_includes(nu_ecs_t ecs, nu_ecs_id_t iter, nu_ecs_id_t c);
NU_API void nu_ecs_excludes(nu_ecs_t ecs, nu_ecs_id_t iter, nu_ecs_id_t c);
NU_API nu_ecs_id_t nu_ecs_begin(nu_ecs_t ecs, nu_ecs_id_t iter);
NU_API nu_ecs_id_t nu_ecs_next(nu_ecs_t ecs, nu_ecs_id_t iter);

#ifdef NU_BUILD_ECS_SERIA

#define NU_ECS_COMPONENT_TYPE_NAME "ecs_component"

NU_API nu_ecs_id_t nu_ecs_register_seria(nu_ecs_t ecs, nu_seria_type_t type);
NU_API nu_seria_buffer_t nu_ecs_write(nu_ecs_t ecs, nu_seria_t seria);
NU_API void              nu_ecs_read(nu_ecs_t          ecs,
                                     nu_seria_t        seria,
                                     nu_seria_buffer_t buffer);
#endif

#endif
