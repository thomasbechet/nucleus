#ifndef NUGL_SHADER_DATA_H
#define NUGL_SHADER_DATA_H
#include <nucleus/core/config.h>
static const nu_char_t *nugl__shader_blit_frag = 
"#version 330 core\n"
"\n"
"out vec4 out_color;\n"
"in vec2 uv;\n"
"\n"
"uniform sampler2D t_surface;\n"
"\n"
"vec2 uv_filtering(in vec2 uv, in vec2 texture_size)\n"
"{\n"
"    vec2 pixel = uv * texture_size;\n"
"    vec2 seam = floor(pixel + 0.5);\n"
"    vec2 dudv = fwidth(pixel);\n"
"    vec2 rel = (pixel - seam) / dudv;\n"
"    vec2 mid_pix = vec2(0.5);\n"
"    pixel = seam + clamp(rel, -mid_pix, mid_pix);\n"
"    return pixel / texture_size;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec2 filtered_uv = uv_filtering(uv, textureSize(t_surface, 0));\n"
"    out_color = texture(t_surface, filtered_uv);\n"
"}\n"
;
static const nu_char_t *nugl__shader_blit_vert = 
"#version 330 core\n"
"\n"
"out vec2 uv;\n"
"\n"
"void main()\n"
"{\n"
"    float x = float((gl_VertexID & 1) << 2);\n"
"    float y = float((gl_VertexID & 2) << 1);\n"
"\n"
"    uv.x = x * 0.5;\n"
"    uv.y = y * 0.5;\n"
"\n"
"    gl_Position = vec4(x - 1.0, y - 1.0, 0.0, 1.0);\n"
"}\n"
;
static const nu_char_t *nugl__shader_canvas_blit_frag = 
"#version 330\n"
"\n"
"in vec2 uv;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D texture0;\n"
"\n"
"void main()\n"
"{\n"
"    color = texture(texture0, uv);\n"
"}\n"
;
static const nu_char_t *nugl__shader_canvas_blit_vert = 
"#version 330\n"
"\n"
"layout(location = 0) in uint pos;\n"
"layout(location = 1) in uint tex;\n"
"layout(location = 2) in uint size;\n"
"layout(location = 3) in float depth;\n"
"\n"
"uniform sampler2D texture0;\n"
"uniform uvec2 viewport_size;\n"
"\n"
"out vec2 uv;\n"
"\n"
"const vec2 offsets[6] = vec2[](\n"
"    vec2(0, 0),\n"
"    vec2(0, 1),\n"
"    vec2(1, 1),\n"
"    vec2(1, 1),\n"
"    vec2(1, 0),\n"
"    vec2(0, 0)\n"
");\n"
"\n"
"void main()\n"
"{\n"
"    // Extract vertex data\n"
"    vec2 vertex_pos = vec2(float(pos & 0xffffu), float(pos >> 16u)); \n"
"    vec2 vertex_tex = vec2(float(tex & 0xffffu), float(tex >> 16u));\n"
"    vec2 vertex_size = vec2(float(size & 0xffffu), float(size >> 16u));\n"
"\n"
"    // Compute vertex offset based on the vertex index\n"
"    vec2 vertex_offset = offsets[gl_VertexID];\n"
"\n"
"    // Apply offset and normalize\n"
"    vec2 position = (vertex_pos + vertex_size * vertex_offset) / vec2(viewport_size); \n"
"    position.y = 1 - position.y;\n"
"    gl_Position = vec4(position * 2 - 1, depth, 1);\n"
"\n"
"    // Set output\n"
"    uv = floor(vertex_tex + vertex_size * vertex_offset) / vec2(textureSize(texture0, 0));\n"
"}\n"
;
static const nu_char_t *nugl__shader_flat_frag = 
"#version 330 core\n"
"\n"
"in vec2 uv;\n"
"in vec3 normal;\n"
"out vec4 frag_color;\n"
"\n"
"uniform sampler2D texture0;\n"
"\n"
"void main()\n"
"{\n"
"    vec4 color = texture(texture0, uv);\n"
"    color *= max(0.3, dot(normal, normalize(vec3(1))));\n"
"    frag_color = color;\n"
"}\n"
;
static const nu_char_t *nugl__shader_flat_vert = 
"#version 330 core\n"
"\n"
"layout(location = 0) in vec3 in_position;\n"
"layout(location = 1) in vec2 in_uv;\n"
"layout(location = 2) in vec3 in_normal;\n"
"\n"
"uniform uvec2 viewport_size;\n"
"uniform mat4 model;\n"
"uniform mat4 view_projection;\n"
"uniform mat3 uv_transform;\n"
"\n"
"uniform sampler2D texture0;\n"
"\n"
"out vec2 uv;\n"
"out vec3 normal;\n"
"\n"
"void main()\n"
"{\n"
"    // Transform vertex\n"
"    vec4 position = view_projection * model * vec4(in_position, 1);\n"
"\n"
"    // Prevent sub pixel aliasing by flooring vertices to pixel\n"
"    vec2 grid = vec2(viewport_size);\n"
"    position.xyz = position.xyz / position.w;\n"
"    position.xy = (floor(grid * position.xy) + 0.5) / grid;\n"
"    position.xyz *= position.w;\n"
"\n"
"    gl_Position = position;\n"
"\n"
"    // Floors UVs\n"
"    uv = (uv_transform * vec3(in_uv, 1)).xy;\n"
"    ivec2 tex_size = textureSize(texture0, 0);\n"
"    uv = (floor(tex_size * uv) + 0.5) / tex_size;\n"
"\n"
"    normal = mat3(transpose(inverse(model))) * in_normal;\n"
"}\n"
;
static const nu_char_t *nugl__shader_skybox_frag = 
"#version 330\n"
"\n"
"uniform samplerCube cubemap;\n"
"\n"
"smooth in vec3 eye_direction;\n"
"\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"    color = texture(cubemap, eye_direction);\n"
"}\n"
;
static const nu_char_t *nugl__shader_skybox_vert = 
"#version 330 core\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat3 rotation;\n"
"\n"
"smooth out vec3 eye_direction;\n"
"\n"
"void main()\n"
"{\n"
"    // Generate fullscreen quad\n"
"    float x = float((gl_VertexID & 1) << 2) - 1;\n"
"    float y = float((gl_VertexID & 2) << 1) - 1;\n"
"    vec4 pos = vec4(x, y, 1, 1);\n"
"\n"
"    // Compute eye direction\n"
"    mat4 inv_proj = inverse(projection);\n"
"    mat3 inv_model_view = transpose(mat3(view));\n"
"    vec3 unprojected = (inv_proj * pos).xyz;\n"
"    eye_direction = rotation * inv_model_view * unprojected;\n"
"\n"
"    gl_Position = pos.xyww; // divide w by w = 1.0 -> max depth\n"
"}\n"
;
#endif
