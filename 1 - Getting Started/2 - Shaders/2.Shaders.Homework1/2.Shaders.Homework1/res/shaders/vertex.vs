#version 330 core
layout (location = 0) in vec3 in_pos;

uniform float u_greenValue;

out vec3 color;

void main()
{
    gl_Position = vec4(in_pos, 1.0);
    color = vec3(0.0, u_greenValue, 0.0);
}