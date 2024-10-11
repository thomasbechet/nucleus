#ifndef NU_UTILS_API_H
#define NU_UTILS_API_H

#include <nucleus/graphics/api.h>

NU_DEFINE_HANDLE(nu_geometry_t);
NU_DEFINE_HANDLE(nu_controller_t);

typedef enum
{
    NU_CONTROLLER_FREEFLY_ALIGNED,
    NU_CONTROLLER_FREEFLY,
    NU_CONTROLLER_CHARACTER
} nu_controller_mode_t;

NU_API nu_controller_t nu_controller_create(nu_input_t view_pitch,
                                            nu_input_t view_yaw,
                                            nu_input_t view_roll,
                                            nu_input_t move_x,
                                            nu_input_t move_y,
                                            nu_input_t move_z,
                                            nu_input_t switch_mode);
NU_API void            nu_controller_delete(nu_controller_t controller);
NU_API void            nu_controller_update(nu_controller_t controller,
                                            nu_f32_t        dt,
                                            nu_camera_t     camera);
NU_API nu_m4_t         nu_controller_transform(nu_controller_t controller);

NU_API nu_geometry_t nu_geometry_create(void);
NU_API void          nu_geometry_delete(nu_geometry_t geometry);
NU_API void          nu_geometry_reset(nu_geometry_t geometry);
NU_API void          nu_geometry_cube(nu_geometry_t geometry, nu_f32_t unit);
NU_API void          nu_geometry_plane(nu_geometry_t geometry,
                                       nu_f32_t      width,
                                       nu_f32_t      height);
NU_API void          nu_geometry_grid(nu_geometry_t geometry,
                                      nu_u32_t      width,
                                      nu_u32_t      height,
                                      nu_f32_t      unit,
                                      nu_f32_t      uv_scale);
NU_API void          nu_geometry_transform(nu_geometry_t geometry, nu_m4_t m);
NU_API void          nu_geometry_merge(nu_geometry_t dst, nu_geometry_t src);
NU_API nu_mesh_t     nu_geometry_create_mesh(nu_geometry_t  geometry,
                                             nu_primitive_t primitive);
NU_API nu_mesh_t     nu_geometry_create_mesh_normals(nu_geometry_t geometry);
NU_API nu_b3_t       nu_geometry_bounds(nu_geometry_t geometry);

NU_API void nu_draw_stats(nu_renderpass_t pass, nu_font_t font, nu_v2i_t pos);

#endif
