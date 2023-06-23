#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main()
{
    FragColor = mix(texture(u_texture1, texCoord), texture(u_texture2, texCoord), 0.2);
}