#version 330

layout(location = 0) in uint pos;
layout(location = 1) in uint tex;
layout(location = 2) in uint size;
layout(location = 3) in float depth;

uniform sampler2D texture0;
uniform uvec2 viewport_size;

out vec2 uv;

const vec2 offsets[6] = vec2[](
    vec2(0, 0),
    vec2(0, 1),
    vec2(1, 1),
    vec2(1, 1),
    vec2(1, 0),
    vec2(0, 0)
);

void main()
{
    // Extract vertex data
    vec2 vertex_pos = vec2(float(pos & 0xffffu), float(pos >> 16u)); 
    vec2 vertex_tex = vec2(float(tex & 0xffffu), float(tex >> 16u));
    vec2 vertex_size = vec2(float(size & 0xffffu), float(size >> 16u));

    // Compute vertex offset based on the vertex index
    vec2 vertex_offset = offsets[gl_VertexID];

    // Apply offset and normalize
    vec2 position = (vertex_pos + vertex_size * vertex_offset) / vec2(viewport_size); 
    position.y = 1 - position.y;
    gl_Position = vec4(position * 2 - 1, depth, 1);

    // Set output
    uv = (vertex_tex + vertex_size * vertex_offset) / vec2(textureSize(texture0, 0));
}
