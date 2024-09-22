#ifndef NUGL_BUFFER_IMPL_H
#define NUGL_BUFFER_IMPL_H

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
        default:
            break;
    }
    NU_ASSERT(NU_FALSE);
    return 0;
}
static nu_size_t
nugl__vertex_divisor (nu_buffer_type_t type)
{
    switch (type)
    {
        case NU_BUFFER_POSITIONS:
            return 1;
        case NU_BUFFER_UVS:
            return 1;
        case NU_BUFFER_NORMALS:
            return 3;
        default:
            break;
    }
    NU_ASSERT(NU_FALSE);
    return 0;
}
static nu_size_t
nugl__float_per_type (nu_buffer_type_t type)
{
    switch (type)
    {
        case NU_BUFFER_POSITIONS:
            return NU_VEC3_SIZE;
        case NU_BUFFER_UVS:
            return NU_VEC2_SIZE;
        case NU_BUFFER_NORMALS:
            return NU_VEC3_SIZE;
        case NU_BUFFER_TRANSFORMS:
            return NU_MAT4_SIZE;
    }
    return 0;
}
static nu_buffer_t
nugl__buffer_create (nu_buffer_type_t type,
                     nu_primitive_t   primitive,
                     nu_size_t        size)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__buffer_t *p      = NU_VEC_PUSH(&gl->buffers);
    nu_buffer_t     handle = NU_HANDLE_MAKE(nu_buffer_t, gl->buffers.size - 1);

    p->size      = size;
    p->type      = type;
    p->primitive = primitive;

    // Create VBO
    glGenBuffers(1, &p->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, p->vbo);

    // Compute buffer size
    nu_size_t bsize = nugl__vertex_per_primitive(primitive) * size
                      * nugl__float_per_type(type) * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, bsize, NU_NULL, GL_STATIC_DRAW);

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return handle;
}
static void
nugl__buffer_delete (nu_buffer_t buffer)
{
    nu__gl_t       *gl = &_ctx.gl;
    nugl__buffer_t *p  = gl->buffers.data + NU_HANDLE_INDEX(buffer);
    glDeleteBuffers(1, &p->vbo);
}
static void
nugl__buffer_vec3 (nu_buffer_t      buffer,
                   nu_size_t        first,
                   nu_size_t        count,
                   const nu_vec3_t *data)
{
    nugl__buffer_t *buf = _ctx.gl.buffers.data + NU_HANDLE_INDEX(buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buf->vbo);
    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    NU_ASSERT(ptr && data);
    NU_ASSERT(first + count <= buf->size);
    NU_ASSERT(nugl__float_per_type(buf->type) == 3);

    const nu_size_t vertex_per_primitive
        = nugl__vertex_per_primitive(buf->primitive);
    const nu_size_t offset  = first * vertex_per_primitive;
    const nu_size_t divisor = nugl__vertex_divisor(buf->type);
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
static void
nugl__buffer_vec2 (nu_buffer_t      buffer,
                   nu_size_t        first,
                   nu_size_t        count,
                   const nu_vec2_t *data)
{
    nugl__buffer_t *buf = _ctx.gl.buffers.data + NU_HANDLE_INDEX(buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buf->vbo);
    float *ptr = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    NU_ASSERT(ptr && data);
    NU_ASSERT(first + count <= buf->size);
    NU_ASSERT(nugl__float_per_type(buf->type) == 2);

    const nu_size_t vertex_per_primitive
        = nugl__vertex_per_primitive(buf->primitive);
    const nu_size_t offset  = first * vertex_per_primitive;
    const nu_size_t divisor = nugl__vertex_divisor(buf->type);
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

#endif
