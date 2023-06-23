#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec2 in_texCoord;

out vec3 color;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(in_pos, 1.0);
    color = in_color;
    texCoord = in_texCoord;
}