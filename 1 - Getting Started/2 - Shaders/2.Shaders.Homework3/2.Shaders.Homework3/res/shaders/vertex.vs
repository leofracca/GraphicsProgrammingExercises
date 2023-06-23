#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

uniform float u_xOffset;

out vec3 color;

void main()
{
    gl_Position = vec4(in_pos.x + u_xOffset, in_pos.y, in_pos.z, 1.0);
    color = in_color;
}