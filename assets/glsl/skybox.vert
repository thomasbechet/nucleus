#version 330 core

uniform mat4 projection;
uniform mat4 view;

smooth out vec3 eye_direction;

void main()
{
    // Generate fullscreen quad
    float x = float((gl_VertexID & 1) << 2) - 1;
    float y = float((gl_VertexID & 2) << 1) - 1;
    vec4 pos = vec4(x, y, 1, 1);

    // Compute eye direction
    mat4 inv_proj = inverse(projection);
    mat3 inv_model_view = transpose(mat3(view));
    vec3 unprojected = (inv_proj * pos).xyz;
    eye_direction = inv_model_view * unprojected;

    gl_Position = pos.xyww; // divide w by w = 1.0 -> max depth
}
