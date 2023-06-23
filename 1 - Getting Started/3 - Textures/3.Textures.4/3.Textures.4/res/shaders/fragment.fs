#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform float     u_mixValue;

void main()
{
    FragColor = mix(texture(u_texture1, texCoord), texture(u_texture2, texCoord), u_mixValue);
}