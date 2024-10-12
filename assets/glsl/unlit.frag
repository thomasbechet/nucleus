#version 330 core

out vec4 frag_color;

uniform sampler2D texture0;

in VS_OUT {
    vec2 uv;
} fs_in;

void main()
{
    frag_color = texture(texture0, fs_in.uv);
}
