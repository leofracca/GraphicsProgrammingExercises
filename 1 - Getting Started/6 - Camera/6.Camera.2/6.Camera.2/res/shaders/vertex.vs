#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_texCoord;

uniform mat4 u_mvp;

out vec2 texCoord;

void main()
{
    gl_Position = u_mvp * vec4(in_pos, 1.0);
    texCoord = in_texCoord;
}