#ifndef NUGL_MESH_IMPL_H
#define NUGL_MESH_IMPL_H

#include <nucleus/internal.h>

static void
nugl__mesh_init (nu__mesh_t *mesh)
{
    nu__gl_t *gl = &_ctx.graphics.gl;

    mesh->gl.positions = 0;
    mesh->gl.uvs       = 0;
    mesh->gl.normals   = 0;

    // Create VAO
    glGenVertexArrays(1, &mesh->gl.vao);
}
static void
nugl__mesh_free (nu__mesh_t *mesh)
{
    glDeleteVertexArrays(1, &mesh->gl.vao);
}
static void
nugl__mesh_write_uvs (nu__mesh_t    *mesh,
                      nu_size_t      first,
                      nu_size_t      count,
                      const nu_v2_t *data)
{
    if (!mesh->gl.uvs)
    {
        glGenBuffers(1, &mesh->gl.uvs);
        glBindVertexArray(mesh->gl.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.uvs);
        nu_size_t bsize
            = nu__primitive_vertex_count(mesh->primitive, mesh->capacity)
              * NU_V2_SIZE * sizeof(nu_f32_t);
        glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V2_SIZE, (void *)0);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.uvs);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr && data);
    NU_ASSERT(first + count <= mesh->capacity);
    const nu_size_t vertex_per_primitive
        = nu__primitive_vertex_count(mesh->primitive, 1);
    const nu_size_t offset = first * vertex_per_primitive;
    for (nu_size_t p = 0; p < count; ++p)
    {
        for (nu_size_t v = 0; v < vertex_per_primitive; ++v)
        {
            const nu_size_t index = p * vertex_per_primitive + v;
            ptr[(offset + index) * NU_V2_SIZE + 0] = data[index].x;
            ptr[(offset + index) * NU_V2_SIZE + 1] = data[index].y;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static void
nugl__mesh_write_positions (nu__mesh_t    *mesh,
                            nu_size_t      first,
                            nu_size_t      count,
                            const nu_v3_t *data)
{
    if (!mesh->gl.positions)
    {
        glBindVertexArray(mesh->gl.vao);

        glGenBuffers(1, &mesh->gl.positions);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.positions);
        nu_size_t bsize
            = nu__primitive_vertex_count(mesh->primitive, mesh->capacity)
              * NU_V3_SIZE * sizeof(nu_f32_t);
        glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V3_SIZE, (void *)0);
        glEnableVertexAttribArray(0);

        if (mesh->primitive == NU_PRIMITIVE_TRIANGLES)
        {
            glGenBuffers(1, &mesh->gl.normals);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.normals);
            bsize = nu__primitive_vertex_count(NU_PRIMITIVE_TRIANGLES,
                                               mesh->capacity)
                    * NU_V3_SIZE * sizeof(nu_f32_t);
            glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(2,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(nu_f32_t) * NU_V3_SIZE,
                                  (void *)0);
            glEnableVertexAttribArray(2);
        }

        glBindVertexArray(0);
    }

    const nu_size_t vcount = nu__primitive_vertex_count(mesh->primitive, count);
    const nu_size_t vfirst = nu__primitive_vertex_first(mesh->primitive, first);

    // Update positions
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.positions);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t v = 0; v < vcount; ++v)
    {
        ptr[(vfirst + v) * NU_V3_SIZE + 0] = data[v].x;
        ptr[(vfirst + v) * NU_V3_SIZE + 1] = data[v].y;
        ptr[(vfirst + v) * NU_V3_SIZE + 2] = data[v].z;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Normals must be updated as well
    if (mesh->primitive == NU_PRIMITIVE_TRIANGLES)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.normals);
        nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        NU_ASSERT(ptr);
        nu_size_t vfirst = nu__primitive_vertex_first(mesh->primitive, first);
        for (nu_size_t p = 0; p < count; ++p)
        {
            const nu_size_t index = p * 3;
            nu_v3_t         p0    = data[index + 0];
            nu_v3_t         p1    = data[index + 1];
            nu_v3_t         p2    = data[index + 2];
            nu_v3_t         n     = nu_triangle_normal(p0, p1, p2);

            for (nu_size_t i = 0; i < 3; ++i)
            {
                ptr[(vfirst + index + i) * NU_V3_SIZE + 0] = n.x;
                ptr[(vfirst + index + i) * NU_V3_SIZE + 1] = n.y;
                ptr[(vfirst + index + i) * NU_V3_SIZE + 2] = n.z;
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif
