#ifndef NUGL_SHADER_H
#define NUGL_SHADER_H
#include <nucleus/types.h>
static const nu_char_t *nugl__shader_blit_frag = 
"#version 330 core\n"
"\n"
"out vec4 color;\n"
"in vec2 uv;\n"
"\n"
"uniform sampler2D t_surface;\n"
"\n"
"void main()\n"
"{\n"
"    color = texture(t_surface, uv);\n"
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
static const nu_char_t *nugl__shader_flat_frag = 
"#version 330 core\n"
"\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"    color = vec4(1, 0, 0, 1);\n"
"}\n"
;
static const nu_char_t *nugl__shader_flat_vert = 
"#version 330 core\n"
"\n"
"out vec2 uv;\n"
"\n"
"layout(location = 0) in vec3 in_position;\n"
"layout(location = 1) in vec2 in_uv;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(in_position, 1.0);\n"
"}\n"
;
#endif
