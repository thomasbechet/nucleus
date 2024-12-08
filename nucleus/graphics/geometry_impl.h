#ifndef NU_GEOMETRY_IMPL_H
#define NU_GEOMETRY_IMPL_H

#include <nucleus/internal.h>

// static nu_mesh_t
// nu__geometry_new_mesh_lines (nu_scope_t scope, nu__geometry_t *g)
// {
//     nu_size_t max_line_count = 0;
//     for (nu_size_t i = 0; i < g->primitives.size; ++i)
//     {
//         nu__primitive_type_t *p = g->primitives.data + i;
//         if (p->count == 2)
//         {
//             max_line_count += p->positions.size / 2;
//         }
//         else if (p->count >= 2)
//         {
//             max_line_count += p->positions.size;
//         }
//     }
//     NU_ASSERT(max_line_count);
//
//     nu_u32_vec_t edges;
//     NU_VEC_INIT(max_line_count * 2, &edges);
//
//     nu_size_t vindex = 0;
//     for (nu_size_t i = 0; i < g->primitives.size; ++i)
//     {
//         nu__primitive_type_t *p = g->primitives.data + i;
//         // Cannot generate triangles for points and lines
//         if (p->count >= 2)
//         {
//             nu_size_t edge_count = p->count;
//             if (p->count == 2)
//             {
//                 edge_count /= 2;
//             }
//             // Iterate over faces
//             const nu_size_t face_count = p->positions.size / p->count;
//             for (nu_size_t f = 0; f < face_count; ++f)
//             {
//                 // Iterate over face edges
//                 for (nu_size_t e = 0; e < edge_count; ++e)
//                 {
//                     // Order edge indices
//                     nu_u32_t index0 = p->positions.data[f * p->count + e +
//                     0]; nu_u32_t index1
//                         = p->positions
//                               .data[f * p->count + (e + 1) % edge_count];
//                     if (index0 > index1)
//                     {
//                         nu_u32_t temp = index1;
//                         index1        = index0;
//                         index0        = temp;
//                     }
//                     // Add edge to list if missing
//                     nu_bool_t found = NU_FALSE;
//                     for (nu_size_t j = 0; j < edges.size; j += 2)
//                     {
//                         if (edges.data[j] == index0
//                             && edges.data[j + 1] == index1)
//                         {
//                             found = NU_TRUE;
//                             break;
//                         }
//                     }
//                     if (!found)
//                     {
//                         *NU_VEC_PUSH(&edges) = index0;
//                         *NU_VEC_PUSH(&edges) = index1;
//                     }
//                 }
//             }
//         }
//     }
//
//     // Generate mesh
//     nu_v3_vec_t positions;
//     NU_VEC_INIT(edges.size, &positions);
//     NU_VEC_RESIZE(&positions, edges.size);
//
//     for (nu_size_t e = 0; e < edges.size; e += 2)
//     {
//         positions.data[e + 0] = g->positions.data[edges.data[e + 0]];
//         positions.data[e + 1] = g->positions.data[edges.data[e + 1]];
//     }
//
//     nu_mesh_t mesh = nu_mesh_new(scope, NU_PRIMITIVE_LINES, edges.size / 2);
//     nu_mesh_set_positions(mesh, 0, edges.size / 2, positions.data);
//
//     NU_VEC_FREE(&edges);
//     NU_VEC_FREE(&positions);
//
//     return mesh;
// }
// static nu_mesh_t
// nu__geometry_new_mesh_triangles (nu_scope_t scope, nu__geometry_t *g)
// {
//     nu_size_t triangle_count = 0;
//     for (nu_size_t i = 0; i < g->primitives.size; ++i)
//     {
//         nu__primitive_type_t *p = g->primitives.data + i;
//         if (p->count >= 3)
//         {
//             triangle_count += p->positions.size / (p->count - 2);
//         }
//     }
//     NU_ASSERT(triangle_count);
//
//     nu_v3_vec_t positions;
//     nu_v2_vec_t uvs;
//     nu_size_t   vertex_count = triangle_count * 3;
//     NU_VEC_INIT(vertex_count, &positions);
//     NU_VEC_INIT(vertex_count, &uvs);
//     NU_VEC_RESIZE(&positions, vertex_count);
//     NU_VEC_RESIZE(&uvs, vertex_count);
//
//     nu_size_t vindex = 0;
//     for (nu_size_t i = 0; i < g->primitives.size; ++i)
//     {
//         nu__primitive_type_t *p = g->primitives.data + i;
//         // Cannot generate triangles for points and lines
//         if (p->count >= 3)
//         {
//             // Iterate over faces
//             const nu_size_t face_count        = p->positions.size / p->count;
//             const nu_size_t triangle_per_face = p->count - 2;
//             for (nu_size_t f = 0; f < face_count; ++f)
//             {
//                 // Generate triangles for a given face
//                 for (nu_size_t t = 0; t < triangle_per_face; ++t)
//                 {
//                     const nu_size_t offset = f * p->count;
//                     positions.data[vindex]
//                         = g->positions.data[p->positions.data[offset + 0]];
//                     uvs.data[vindex] = g->uvs.data[p->uvs.data[offset + 0]];
//                     ++vindex;
//                     positions.data[vindex]
//                         = g->positions.data[p->positions.data[offset + 1 +
//                         t]];
//                     uvs.data[vindex] = g->uvs.data[p->uvs.data[offset + 1 +
//                     t]];
//                     ++vindex;
//                     positions.data[vindex]
//                         = g->positions.data[p->positions.data[offset + 2 +
//                         t]];
//                     uvs.data[vindex] = g->uvs.data[p->uvs.data[offset + 2 +
//                     t]];
//                     ++vindex;
//                 }
//             }
//         }
//     }
//
//     nu_mesh_t mesh = nu_mesh_new(scope, NU_PRIMITIVE_TRIANGLES,
//     triangle_count); nu_mesh_set_positions(mesh, 0, triangle_count,
//     positions.data); nu_mesh_set_uvs(mesh, 0, triangle_count, uvs.data);
//
//     NU_VEC_FREE(&positions);
//     NU_VEC_FREE(&uvs);
//
//     return mesh;
// }
#ifdef NU_BUILD_UTILS_SERIA
void
nu_geometry_load (nu_geometry_t geometry, nu_seria_t seria)
{
    nu__geometry_t *g = (nu__geometry_t *)geometry;
}
void
nu_geometry_save (nu_geometry_t geometry, nu_seria_t seria)
{
}
#endif

#endif
