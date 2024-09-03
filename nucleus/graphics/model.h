#ifndef NU_MODEL_H
#define NU_MODEL_H

#include <nucleus/graphics/renderpass.h>

NU_DEFINE_HANDLE(nu_model_t);

NU_API void nu_model_create(nu_model_t *model);
NU_API void nu_model_delete(nu_model_t model);
NU_API void nu_draw_model(nu_renderpass_t pass,
                          nu_model_t      model,
                          nu_mat4_t       transform);

#endif
