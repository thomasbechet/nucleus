#ifndef NU_CAMERA_H
#define NU_CAMERA_H

#include <nucleus/core/core.h>

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

NU_DEFINE_HANDLE(nu_camera_t);

#endif
