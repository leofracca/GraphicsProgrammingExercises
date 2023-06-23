#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 u_viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material u_material;
in vec2 texCoords; // For the texture of the diffuse

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light u_light;

void main()
{
    // Ambient light
    vec3 ambient = u_light.ambient * vec3(texture(u_material.diffuse, texCoords));

    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-u_light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_light.diffuse * diff * vec3(texture(u_material.diffuse, texCoords));

    // Specular light
    vec3 viewDir = normalize(u_viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * spec * vec3(texture(u_material.specular, texCoords));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}