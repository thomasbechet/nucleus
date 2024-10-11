#ifndef NUGL_MESH_IMPL_H
#define NUGL_MESH_IMPL_H

#include <nucleus/internal.h>

static nu_mesh_t
nugl__mesh_create (nu_primitive_t primitive, nu_size_t capacity)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__mesh_t *p      = NU_VEC_PUSH(&gl->meshes);
    nu_mesh_t     handle = NU_HANDLE_MAKE(nu_mesh_t, gl->meshes.size - 1);

    p->primitive = primitive;
    p->capacity  = capacity;
    p->positions = 0;
    p->uvs       = 0;
    p->normals   = 0;

    // Create VAO
    glGenVertexArrays(1, &p->vao);

    return handle;
}
static void
nugl__mesh_delete (nu_mesh_t mesh)
{
    nu__gl_t     *gl    = &_ctx.gl;
    nugl__mesh_t *pmesh = gl->meshes.data + NU_HANDLE_INDEX(mesh);
    glDeleteVertexArrays(1, &pmesh->vao);
}
static nu_size_t
nugl__mesh_capacity (nu_mesh_t mesh)
{
    return _ctx.gl.meshes.data[NU_HANDLE_INDEX(mesh)].capacity;
}
static nu_primitive_t
nugl__mesh_primitive (nu_mesh_t mesh)
{
    return _ctx.gl.meshes.data[NU_HANDLE_INDEX(mesh)].primitive;
}
static void
nugl__mesh_write_uvs (nu_mesh_t      mesh,
                      nu_size_t      first,
                      nu_size_t      count,
                      const nu_v2_t *data)
{
    NU_ASSERT(mesh);
    nugl__mesh_t *p = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);

    if (!p->uvs)
    {
        glGenBuffers(1, &p->uvs);
        glBindVertexArray(p->vao);
        glBindBuffer(GL_ARRAY_BUFFER, p->uvs);
        nu_size_t bsize = nu__primitive_vertex_count(p->primitive, p->capacity)
                          * NU_V2_SIZE * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * NU_V2_SIZE, (void *)0);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, p->uvs);
    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr && data);
    NU_ASSERT(first + count <= p->capacity);
    const nu_size_t vertex_per_primitive
        = nu__primitive_vertex_count(p->primitive, 1);
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
nugl__mesh_write_positions (nu_mesh_t      mesh,
                            nu_size_t      first,
                            nu_size_t      count,
                            const nu_v3_t *data)
{
    NU_ASSERT(mesh);
    nugl__mesh_t *p = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    NU_ASSERT(first + count <= p->capacity);

    if (!p->positions)
    {
        glBindVertexArray(p->vao);

        glGenBuffers(1, &p->positions);
        glBindBuffer(GL_ARRAY_BUFFER, p->positions);
        nu_size_t bsize = nu__primitive_vertex_count(p->primitive, p->capacity)
                          * NU_V3_SIZE * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * NU_V3_SIZE, (void *)0);
        glEnableVertexAttribArray(0);

        if (p->primitive == NU_PRIMITIVE_TRIANGLES)
        {
            glGenBuffers(1, &p->normals);
            glBindBuffer(GL_ARRAY_BUFFER, p->normals);
            bsize = nu__primitive_vertex_count(NU_PRIMITIVE_TRIANGLES,
                                               p->capacity)
                    * NU_V3_SIZE * sizeof(float);
            glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(2,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(float) * NU_V3_SIZE,
                                  (void *)0);
            glEnableVertexAttribArray(2);
        }

        glBindVertexArray(0);
    }

    const nu_size_t vcount = nu__primitive_vertex_count(p->primitive, count);
    const nu_size_t vfirst = nu__primitive_vertex_first(p->primitive, first);

    // Update positions
    glBindBuffer(GL_ARRAY_BUFFER, p->positions);
    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr);
    for (nu_size_t v = 0; v < vcount; ++v)
    {
        ptr[(vfirst + v) * NU_V3_SIZE + 0] = data[v].x;
        ptr[(vfirst + v) * NU_V3_SIZE + 1] = data[v].y;
        ptr[(vfirst + v) * NU_V3_SIZE + 2] = data[v].z;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Normals must be updated as well
    if (p->primitive == NU_PRIMITIVE_TRIANGLES)
    {
        glBindBuffer(GL_ARRAY_BUFFER, p->normals);
        float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        NU_ASSERT(ptr);
        nu_size_t vfirst = nu__primitive_vertex_first(p->primitive, first);
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
