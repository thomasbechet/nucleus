#ifndef NU_RESOURCE_API_H
#define NU_RESOURCE_API_H

#include <nucleus/seria/api.h>

typedef enum
{
    NU_RESOURCE_CREATE,
    NU_RESOURCE_DELETE,
    NU_RESOURCE_LOAD,
    NU_RESOURCE_SAVE
} nu_resource_action_t;

typedef void *(*nu_resource_handler_t)(nu_resource_action_t action,
                                       nu_uid_t             type,
                                       void                *data,
                                       nu_seria_t           seria);

NU_API void nu_resource_register(nu_uid_t              uid,
                                 nu_str_t              name,
                                 nu_resource_handler_t handler);

NU_API void  nu_resource_create(nu_uid_t type,
                                nu_uid_t group,
                                nu_uid_t uid,
                                void    *data);
NU_API void  nu_resource_delete(nu_uid_t uid);
NU_API void *nu_resource(nu_uid_t type, nu_uid_t uid);

NU_API nu_uid_t nu_resource_load_group(nu_seria_t seria);
NU_API void     nu_resource_save_group(nu_uid_t group, nu_seria_t seria);
NU_API void     nu_resource_delete_group(nu_uid_t group);

#define NU_RESOURCE_IMAGE   NU_UID("image")
#define NU_RESOURCE_TEXTURE NU_UID("texture")
#define NU_RESOURCE_MODEL   NU_UID("model")
#define NU_RESOURCE_ECS     NU_UID("ecs")
#define NU_RESOURCE_INPUT   NU_UID("input")

#endif
