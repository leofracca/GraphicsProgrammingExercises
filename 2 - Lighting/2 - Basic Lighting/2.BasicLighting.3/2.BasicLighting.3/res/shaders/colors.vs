#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_mvp;

uniform vec3 u_viewPos;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;

out vec3 lightColor;

void main()
{
    gl_Position = u_mvp * vec4(in_pos, 1.0);

    vec3 position = vec3(u_model * vec4(in_pos, 1.0));
    vec3 normal = mat3(transpose(inverse(u_model))) * in_normal;

    // Ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_lightColor;

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(u_lightPos - position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // Specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_viewPos - position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_lightColor;

    lightColor = ambient + diffuse + specular;
}