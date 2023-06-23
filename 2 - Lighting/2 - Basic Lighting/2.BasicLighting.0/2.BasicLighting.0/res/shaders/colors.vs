#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

uniform mat4 u_model;
uniform mat4 u_mvp;

out vec3 fragPos;
out vec3 normal;

void main()
{
    gl_Position = u_mvp * vec4(in_pos, 1.0);
    fragPos = vec3(u_model * vec4(in_pos, 1.0));
    normal = mat3(transpose(inverse(u_model))) * in_normal;
}