#ifndef NU_GEOMETRY_H
#define NU_GEOMETRY_H

#include <nucleus/graphics/graphics.h>

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
