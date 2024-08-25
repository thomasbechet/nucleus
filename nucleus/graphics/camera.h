#ifndef NU_CAMERA_H
#define NU_CAMERA_H

#include <nucleus/core.h>

typedef enum
{
    NU_PROJECTION_PERSPECTIVE,
    NU_PROJECTION_ORTHOGRAPHIC,
} nu_projection_t;

typedef struct
{
    nu_projection_t projection;
    float           fov;
    float           near;
    float           far;
    nu_vec3_t       up;
    nu_vec3_t       center;
    nu_vec3_t       eye;
} nu_camera_info_t;

#define NU_CAMERA_INFO_DEFAULT                                            \
    (nu_camera_info_t)                                                    \
    {                                                                     \
        .projection = NU_PROJECTION_PERSPECTIVE, .fov = 80, .near = 0.01, \
        .far = 100, .eye = NU_VEC3_ZERO, .center = NU_VEC3_FORWARD,       \
        .up = NU_VEC3_UP                                                  \
    }

NU_DEFINE_HANDLE(nu_camera_handle_t);

#endif
