#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main()
{
    FragColor = mix(texture(u_texture1, texCoord), texture(u_texture2, vec2(1.0 - texCoord.x, texCoord.y)), 0.5);
}