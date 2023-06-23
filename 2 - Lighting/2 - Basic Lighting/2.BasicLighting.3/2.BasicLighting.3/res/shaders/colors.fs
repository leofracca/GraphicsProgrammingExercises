#version 330 core

in vec3 lightColor;

out vec4 FragColor;

uniform vec3 u_objectColor;

void main()
{
    FragColor = vec4(lightColor * u_objectColor, 1.0);
}