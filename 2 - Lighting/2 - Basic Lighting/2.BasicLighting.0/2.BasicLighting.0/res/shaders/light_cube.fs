#version 330 core
out vec4 FragColor;

uniform vec3 u_objectColor;
uniform vec3 u_lightColor;

void main()
{
    // Output only white color (the color of the light)
    FragColor = vec4(1.0f);
}