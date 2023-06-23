#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 u_viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material u_material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light u_light;

void main()
{
    // Ambient light
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * u_lightColor;
    vec3 ambient = u_light.ambient * u_material.ambient;

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(u_light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = diff * u_lightColor;
    vec3 diffuse = u_light.diffuse * (diff * u_material.diffuse);

    // Specular light
    // float specularStrength = 0.5;
    vec3 viewDir = normalize(u_viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    // vec3 specular = specularStrength * spec * u_lightColor;
    vec3 specular = u_light.specular * (spec * u_material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}