#ifndef NUGL_SHADER_H
#define NUGL_SHADER_H
#include <nucleus/types.h>
static const nu_char_t *nugl__shader_blit_frag = 
"#version 330 core\n"
"\n"
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"    color = vec4(0, 0, 1, 1);\n"
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
#endif
