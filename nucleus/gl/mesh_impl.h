#ifndef NUGL_MESH_IMPL_H
#define NUGL_MESH_IMPL_H

#include <nucleus/internal.h>

static nu_size_t
nugl__vertex_per_primitive (nu_primitive_t primitive)
{
    switch (primitive)
    {
        case NU_PRIMITIVE_POINTS:
            return 1;
        case NU_PRIMITIVE_TRIANGLES:
            return 3;
        case NU_PRIMITIVE_LINES:
            return 2;
    }
    return 0;
}
static nu_size_t
nugl__vertex_divisor (nu_mesh_buffer_t type)
{
    switch (type)
    {
        case NU_MESH_POSITIONS:
            return 1;
        case NU_MESH_UVS:
            return 1;
        case NU_MESH_NORMALS:
            return 3;
    }
    return 0;
}
static nu_size_t
nugl__float_per_type (nu_mesh_buffer_t type)
{
    switch (type)
    {
        case NU_MESH_POSITIONS:
            return NU_VEC3_SIZE;
        case NU_MESH_UVS:
            return NU_VEC2_SIZE;
        case NU_MESH_NORMALS:
            return NU_VEC3_SIZE;
    }
    return 0;
}

static nu_mesh_t
nugl__mesh_create (nu_primitive_t primitive, nu_size_t capacity)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__mesh_t *p      = NU_VEC_PUSH(&gl->meshes);
    nu_mesh_t     handle = NU_HANDLE_MAKE(nu_mesh_t, gl->meshes.size - 1);

    p->primitive      = primitive;
    p->capacity       = capacity;
    p->count          = capacity;
    p->positions      = 0;
    p->first_position = 0;
    p->uvs            = 0;
    p->first_uv       = 0;
    p->normals        = 0;
    p->first_normal   = 0;

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
static void
nugl__mesh_buffer_vec2 (nu_mesh_t        mesh,
                        nu_mesh_buffer_t buffer,
                        nu_size_t        first,
                        nu_size_t        count,
                        const nu_vec2_t *data)
{
    NU_ASSERT(mesh);
    nugl__mesh_t *p = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    switch (buffer)
    {
        case NU_MESH_UVS: {
            if (!p->uvs)
            {
                glGenBuffers(1, &p->uvs);
                glBindVertexArray(p->vao);
                glBindBuffer(GL_ARRAY_BUFFER, p->uvs);
                nu_size_t bsize = nugl__vertex_per_primitive(p->primitive)
                                  * p->capacity * nugl__float_per_type(buffer)
                                  * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_STATIC_DRAW);
                glVertexAttribPointer(1,
                                      2,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(float) * NU_VEC2_SIZE,
                                      (void *)0);
                glEnableVertexAttribArray(1);
                glBindVertexArray(0);
            }
            else
            {
                glBindBuffer(GL_ARRAY_BUFFER, p->uvs);
            }
        }
        break;
        default:
            NU_ASSERT(NU_FALSE);
            break;
    }

    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr && data);
    NU_ASSERT(first + count <= p->capacity);
    NU_ASSERT(nugl__float_per_type(buffer) == 2);
    const nu_size_t vertex_per_primitive
        = nugl__vertex_per_primitive(p->primitive);
    const nu_size_t offset  = first * vertex_per_primitive;
    const nu_size_t divisor = nugl__vertex_divisor(buffer);
    for (nu_size_t p = 0; p < count; ++p)
    {
        for (nu_size_t v = 0; v < vertex_per_primitive; ++v)
        {
            const nu_size_t index = p * vertex_per_primitive + v;
            ptr[(offset + index) * NU_VEC2_SIZE + 0] = data[index / divisor].x;
            ptr[(offset + index) * NU_VEC2_SIZE + 1] = data[index / divisor].y;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
static void
nugl__mesh_buffer_vec3 (nu_mesh_t        mesh,
                        nu_mesh_buffer_t buffer,
                        nu_size_t        first,
                        nu_size_t        count,
                        const nu_vec3_t *data)
{
    NU_ASSERT(mesh);
    nugl__mesh_t *p = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    switch (buffer)
    {
        case NU_MESH_POSITIONS: {
            if (!p->positions)
            {
                glGenBuffers(1, &p->positions);
                glBindVertexArray(p->vao);
                glBindBuffer(GL_ARRAY_BUFFER, p->positions);
                nu_size_t bsize = nugl__vertex_per_primitive(p->primitive)
                                  * p->capacity * nugl__float_per_type(buffer)
                                  * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_STATIC_DRAW);
                glVertexAttribPointer(0,
                                      3,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(float) * NU_VEC3_SIZE,
                                      (void *)0);
                glEnableVertexAttribArray(0);
                glBindVertexArray(0);
            }
            else
            {
                glBindBuffer(GL_ARRAY_BUFFER, p->uvs);
            }
        }
        break;
        case NU_MESH_NORMALS: {
            if (!p->normals)
            {
                glGenBuffers(1, &p->normals);
                glBindVertexArray(p->vao);
                glBindBuffer(GL_ARRAY_BUFFER, p->normals);
                nu_size_t bsize = nugl__vertex_per_primitive(p->primitive)
                                  * p->capacity * nugl__float_per_type(buffer)
                                  * sizeof(float);
                glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_STATIC_DRAW);
                glVertexAttribPointer(2,
                                      3,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(float) * NU_VEC3_SIZE,
                                      (void *)0);
                glEnableVertexAttribArray(2);
                glBindVertexArray(0);
            }
            else
            {
                glBindBuffer(GL_ARRAY_BUFFER, p->uvs);
            }
        }
        break;
        default:
            NU_ASSERT(NU_FALSE);
            break;
    }

    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    NU_ASSERT(ptr && data);
    NU_ASSERT(first + count <= p->capacity);
    NU_ASSERT(nugl__float_per_type(buffer) == 3);
    const nu_size_t vertex_per_primitive
        = nugl__vertex_per_primitive(p->primitive);
    const nu_size_t offset  = first * vertex_per_primitive;
    const nu_size_t divisor = nugl__vertex_divisor(buffer);
    for (nu_size_t p = 0; p < count; ++p)
    {
        for (nu_size_t v = 0; v < vertex_per_primitive; ++v)
        {
            const nu_size_t index = p * vertex_per_primitive + v;
            ptr[(offset + index) * NU_VEC3_SIZE + 0] = data[index / divisor].x;
            ptr[(offset + index) * NU_VEC3_SIZE + 1] = data[index / divisor].y;
            ptr[(offset + index) * NU_VEC3_SIZE + 2] = data[index / divisor].z;
        }
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif
