#ifndef NUGL_MESH_IMPL_H
#define NUGL_MESH_IMPL_H

#include <nucleus/internal.h>

static nu_mesh_t
nugl__mesh_create (nu_primitive_t primitive, nu_size_t count)
{
    nu__gl_t *gl = &_ctx.gl;

    nugl__mesh_t *p      = NU_VEC_PUSH(&gl->meshes);
    nu_mesh_t     handle = NU_HANDLE_MAKE(nu_mesh_t, gl->meshes.size - 1);

    p->primitive      = primitive;
    p->count          = count;
    p->positions      = NU_NULL;
    p->first_position = 0;
    p->uvs            = NU_NULL;
    p->first_uv       = 0;
    p->normals        = NU_NULL;
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
nugl__mesh_buffer (nu_mesh_t mesh, nu_buffer_t buffer, nu_size_t first)
{
    NU_ASSERT(mesh && buffer);
    nugl__mesh_t   *pmesh   = _ctx.gl.meshes.data + NU_HANDLE_INDEX(mesh);
    nugl__buffer_t *pbuffer = _ctx.gl.buffers.data + NU_HANDLE_INDEX(buffer);

    NU_ASSERT(pmesh->primitive == pbuffer->primitive);

    // Bind buffers
    glBindVertexArray(pmesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, pbuffer->vbo);

    switch (pbuffer->type)
    {
        case NU_BUFFER_POSITIONS: {
            if (pmesh->positions != buffer)
            {
                glVertexAttribPointer(0,
                                      3,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(float) * NU_VEC3_SIZE,
                                      (void *)0);
                glEnableVertexAttribArray(0);
            }
            pmesh->positions      = buffer;
            pmesh->first_position = first;
        }
        break;
        case NU_BUFFER_UVS: {
            if (pmesh->uvs != buffer)
            {
                glVertexAttribPointer(1,
                                      2,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(float) * NU_VEC2_SIZE,
                                      (void *)0);
                glEnableVertexAttribArray(1);
            }
            pmesh->uvs      = buffer;
            pmesh->first_uv = first;
        }
        break;
        case NU_BUFFER_NORMALS: {
            if (pmesh->normals != buffer)
            {
                glVertexAttribPointer(2,
                                      3,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(float) * NU_VEC3_SIZE,
                                      (void *)0);
                glEnableVertexAttribArray(2);
            }
            pmesh->normals      = buffer;
            pmesh->first_normal = first;
        }
        break;
        case NU_BUFFER_TRANSFORMS:
            NU_ERROR("invalid mesh buffer binding");
            break;
    }

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

#endif
