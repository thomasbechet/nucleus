#ifndef NU_UTILS_API_H
#define NU_UTILS_API_H

#include <nucleus/platform/api.h>
#include <nucleus/graphics/api.h>
#ifdef NU_BUILD_UTILS_SERIA
#include <nucleus/seria/api.h>
#endif

NU_DEFINE_OBJECT(nu_controller_t);

typedef enum
{
    NU_CONTROLLER_FREEFLY_ALIGNED,
    NU_CONTROLLER_FREEFLY,
    NU_CONTROLLER_CHARACTER
} nu_controller_mode_t;

NU_API nu_controller_t nu_controller_new(nu_input_t view_pitch,
                                         nu_input_t view_yaw,
                                         nu_input_t view_roll,
                                         nu_input_t move_x,
                                         nu_input_t move_y,
                                         nu_input_t move_z,
                                         nu_input_t switch_mode);
NU_API void            nu_controller_update(nu_controller_t controller,
                                            nu_f32_t        dt,
                                            nu_camera_t     camera);
NU_API nu_m4_t         nu_controller_transform(nu_controller_t controller);

NU_API void nu_draw_stats(nu_renderpass_t pass, nu_font_t font, nu_v2i_t pos);

#endif
