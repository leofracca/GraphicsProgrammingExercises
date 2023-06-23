#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec3 lightPosInViewSpace;

out vec4 FragColor;

uniform vec3 u_objectColor;
uniform vec3 u_lightColor;

void main()
{
    // Ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_lightColor;

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosInViewSpace - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // Specular light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * u_objectColor;
    FragColor = vec4(result, 1.0);
}