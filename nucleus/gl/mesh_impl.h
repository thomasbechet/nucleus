#ifndef NUGL_MESH_IMPL_H
#define NUGL_MESH_IMPL_H

#include <nucleus/internal.h>

static nu_mesh_t
nugl__mesh_create (nu_size_t count)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__mesh_t *pmesh  = NU_VEC_PUSH(&gl->meshes);
    nu_mesh_t     handle = NU_HANDLE_MAKE(nu_mesh_t, gl->meshes.size - 1);

    pmesh->vertex_count = count;

    // Create VAO
    glGenVertexArrays(1, &pmesh->vao);
    glBindVertexArray(pmesh->vao);

    // Create VBO
    glGenBuffers(1, &pmesh->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, pmesh->vbo);

    // Format vertices
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * count * NUGL__VERTEX_SIZE,
                 NU_NULL,
                 GL_STATIC_DRAW);

    // Configure VAO
    // Position
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * NUGL__VERTEX_SIZE, (void *)0);
    glEnableVertexAttribArray(0);
    // UV
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * NUGL__VERTEX_SIZE,
                          (void *)(sizeof(float) * NU_VEC3_SIZE));
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * NUGL__VERTEX_SIZE,
        (void *)(sizeof(float) * (NU_VEC3_SIZE + NU_VEC2_SIZE)));
    glEnableVertexAttribArray(2);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return handle;
}
static void
nugl__mesh_delete (nu_mesh_t mesh)
{
    nu__gl_t     *gl    = &_ctx.gl;
    nugl__mesh_t *pmesh = gl->meshes.data + NU_HANDLE_INDEX(mesh);
    glDeleteVertexArrays(1, &pmesh->vao);
    glDeleteBuffers(1, &pmesh->vbo);
}
static void
nugl__mesh_update (nu_mesh_t        mesh,
                   const nu_vec3_t *positions,
                   const nu_vec2_t *uvs,
                   const nu_vec3_t *normals)
{
    nugl__mesh_t *pmesh = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    glBindBuffer(GL_ARRAY_BUFFER, pmesh->vbo);
    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t i = 0; i < pmesh->vertex_count; ++i)
    {
        if (positions)
        {
            ptr[i * NUGL__VERTEX_SIZE + 0] = positions[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 1] = positions[i].y;
            ptr[i * NUGL__VERTEX_SIZE + 2] = positions[i].z;
        }
        if (uvs)
        {
            ptr[i * NUGL__VERTEX_SIZE + 3] = uvs[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 4] = uvs[i].y;
        }
        if (normals)
        {
            ptr[i * NUGL__VERTEX_SIZE + 5] = normals[i].x;
            ptr[i * NUGL__VERTEX_SIZE + 6] = normals[i].y;
            ptr[i * NUGL__VERTEX_SIZE + 7] = normals[i].z;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif
