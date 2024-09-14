#ifndef NU_UTILS_API_H
#define NU_UTILS_API_H

#include <nucleus/graphics/api.h>


typedef enum
{
    NU_CONTROLLER_FREEFLY_ALIGNED,
    NU_CONTROLLER_FREEFLY,
    NU_CONTROLLER_CHARACTER
} nu_controller_mode_t;

typedef struct
{
    nu_input_t view_pitch_neg;
    nu_input_t view_pitch_pos;
    nu_input_t view_yaw_neg;
    nu_input_t view_yaw_pos;
    nu_input_t view_roll_neg;
    nu_input_t view_roll_pos;
    nu_input_t move_up;
    nu_input_t move_down;
    nu_input_t move_left;
    nu_input_t move_right;
    nu_input_t move_forward;
    nu_input_t move_backward;
    nu_input_t switch_mode;
} nu_controller_info_t;

NU_DEFINE_HANDLE(nu_controller_t);

NU_API nu_controller_t nu_controller_create(const nu_controller_info_t *info);
NU_API void            nu_controller_delete(nu_controller_t controller);
NU_API void            nu_controller_update(nu_controller_t controller,
                                            float           dt,
                                            nu_camera_t     camera);

#define NU_CUBE_VERTEX_COUNT  36
#define NU_PLANE_VERTEX_COUNT 6

NU_DEFINE_HANDLE(nu_geometry_t);

NU_API nu_geometry_t nu_geometry_create(nu_size_t capacity);
NU_API void          nu_geometry_delete(nu_geometry_t geometry);

NU_API void nu_geometry_cube(nu_geometry_t geometry, float unit);
NU_API void nu_geometry_plane(nu_geometry_t geometry,
                              float         width,
                              float         height);
NU_API void nu_geometry_grid(nu_geometry_t geometry,
                             nu_u32_t      width,
                             nu_u32_t      height,
                             float         unit,
                             float         uv_scale);
NU_API void nu_geometry_transform(nu_geometry_t geometry, nu_mat4_t m);
NU_API void nu_geometry_append(nu_geometry_t dst, nu_geometry_t src);

NU_API nu_mesh_t nu_mesh_create_geometry(nu_geometry_t geometry);

#endif
