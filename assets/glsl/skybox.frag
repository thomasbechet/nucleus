#version 330

uniform samplerCube cubemap;

smooth in vec3 eye_direction;

out vec4 color;

void main()
{
    color = texture(cubemap, eye_direction);
}
