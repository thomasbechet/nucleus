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
    if (mesh->gl.positions)
    {
        glDeleteBuffers(1, &mesh->gl.positions);
    }
    if (mesh->gl.normals)
    {
        glDeleteBuffers(1, &mesh->gl.normals);
    }
    if (mesh->gl.uvs)
    {
        glDeleteBuffers(1, &mesh->gl.uvs);
    }
}
static void
nugl__mesh_write_uvs (nu__mesh_t *mesh, nu_size_t first, nu_size_t count)
{
    if (!mesh->gl.uvs)
    {
        glGenBuffers(1, &mesh->gl.uvs);
        glBindVertexArray(mesh->gl.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.uvs);
        glBufferData(GL_ARRAY_BUFFER,
                     mesh->capacity * NU_V2_SIZE * sizeof(nu_f32_t),
                     NU_NULL,
                     GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V2_SIZE, (void *)0);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.uvs);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    NU_ASSERT(first + count <= mesh->size);
    for (nu_size_t p = 0; p < count; ++p)
    {
        ptr[(first + p) * NU_V2_SIZE + 0] = mesh->uvs[p].x;
        ptr[(first + p) * NU_V2_SIZE + 1] = mesh->uvs[p].y;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static void
nugl__mesh_write_positions (nu__mesh_t *mesh, nu_size_t first, nu_size_t count)
{
    if (!mesh->gl.positions)
    {
        glBindVertexArray(mesh->gl.vao);

        glGenBuffers(1, &mesh->gl.positions);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.positions);
        glBufferData(GL_ARRAY_BUFFER,
                     mesh->capacity * NU_V3_SIZE * sizeof(nu_f32_t),
                     NU_NULL,
                     GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, sizeof(nu_f32_t) * NU_V3_SIZE, (void *)0);
        glEnableVertexAttribArray(0);

        if (mesh->primitive == NU_PRIMITIVE_TRIANGLES)
        {
            glGenBuffers(1, &mesh->gl.normals);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.normals);
            glBufferData(GL_ARRAY_BUFFER,
                         mesh->capacity * NU_V3_SIZE * sizeof(nu_f32_t),
                         NU_NULL,
                         GL_DYNAMIC_DRAW);
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

    // Update positions
    glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.positions);
    nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < count; ++i)
    {
        ptr[(first + i) * NU_V3_SIZE + 0] = mesh->positions[i].x;
        ptr[(first + i) * NU_V3_SIZE + 1] = mesh->positions[i].y;
        ptr[(first + i) * NU_V3_SIZE + 2] = mesh->positions[i].z;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Normals must be updated as well
    if (mesh->primitive == NU_PRIMITIVE_TRIANGLES)
    {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->gl.normals);
        nu_f32_t *ptr = (nu_f32_t *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        NU_ASSERT(ptr);
        nu_size_t tricount = count / 3;
        for (nu_size_t t = 0; t < tricount; ++t)
        {
            const nu_size_t index = t * 3;
            nu_v3_t         p0    = mesh->positions[index + 0];
            nu_v3_t         p1    = mesh->positions[index + 1];
            nu_v3_t         p2    = mesh->positions[index + 2];
            nu_v3_t         n     = nu_triangle_normal(p0, p1, p2);

            for (nu_size_t i = 0; i < 3; ++i)
            {
                ptr[(first + index + i) * NU_V3_SIZE + 0] = n.x;
                ptr[(first + index + i) * NU_V3_SIZE + 1] = n.y;
                ptr[(first + index + i) * NU_V3_SIZE + 2] = n.z;
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif
