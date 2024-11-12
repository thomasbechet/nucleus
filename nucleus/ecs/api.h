#ifndef NU_ECS_API_H
#define NU_ECS_API_H

#include <nucleus/seria/api.h>
#ifdef NU_BUILD_RESOURCE
#include <nucleus/resource/api.h>
#endif

NU_DEFINE_HANDLE(nu_ecs_t);

#define NU_ECS_ITER(ecs, iter, e)                           \
    for (nu_ecs_id_t(e) = nu_ecs_begin((ecs), (iter)); (e); \
         (e)            = nu_ecs_next((ecs), (iter)))

#define NU_ECS_REG_COMPONENT(name, layout) \
    nu_ecs_register(NU_STR(#name), NU_SERIA_LAYOUT(layout))

#define NU_ECS_COMPONENT(name) nu_ecs_find_component(NU_STR(#name))

typedef nu_u32_t nu_ecs_id_t;

NU_API nu_ecs_t nu_ecs_create(void);
NU_API void     nu_ecs_delete(nu_ecs_t ecs);

NU_API nu_ecs_id_t nu_ecs_register(nu_str_t name, nu_seria_layout_t layout);
NU_API nu_ecs_id_t nu_ecs_find_component(nu_str_t name);

NU_API nu_ecs_id_t nu_ecs_add(nu_ecs_t ecs);
NU_API void        nu_ecs_add_at(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API void        nu_ecs_remove(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API nu_bool_t   nu_ecs_valid(nu_ecs_t ecs, nu_ecs_id_t e);
NU_API nu_size_t   nu_ecs_count(nu_ecs_t ecs);
NU_API nu_size_t   nu_ecs_capacity(nu_ecs_t ecs);
NU_API void        nu_ecs_clear(nu_ecs_t ecs);

NU_API void     *nu_ecs_set(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void      nu_ecs_unset(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API void     *nu_ecs_get(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);
NU_API nu_bool_t nu_ecs_has(nu_ecs_t ecs, nu_ecs_id_t e, nu_ecs_id_t c);

NU_API nu_ecs_id_t nu_ecs_iter(nu_ecs_t ecs);
NU_API void nu_ecs_includes(nu_ecs_t ecs, nu_ecs_id_t iter, nu_ecs_id_t c);
NU_API void nu_ecs_excludes(nu_ecs_t ecs, nu_ecs_id_t iter, nu_ecs_id_t c);
NU_API nu_ecs_id_t nu_ecs_begin(nu_ecs_t ecs, nu_ecs_id_t iter);
NU_API nu_ecs_id_t nu_ecs_next(nu_ecs_t ecs, nu_ecs_id_t iter);

NU_API void     nu_ecs_save(nu_ecs_t ecs, nu_seria_t seria);
NU_API nu_ecs_t nu_ecs_load(nu_seria_t seria);
NU_API void     nu_ecs_dump(nu_ecs_t ecs);

#ifdef NU_BUILD_RESOURCE
NU_API nu_ecs_t nu_ecs(nu_uid_t uid);
#endif

#endif
