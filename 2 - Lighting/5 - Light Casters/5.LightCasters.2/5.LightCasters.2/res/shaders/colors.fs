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
    // The following 3 are used to implement a spotlight
    vec3 position;
    vec3 direction;
    float cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Attenuation factors
    float constant;
    float linear;
    float quadratic;
};
uniform Light u_light;

void main()
{
    vec3 lightDir = normalize(u_light.position - fragPos);
    
    // Check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-u_light.direction));

    if (theta > u_light.cutoff)
    {
        // Attenuation
        float distance    = length(u_light.position - fragPos);
        float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));

        // Ambient light
        vec3 ambient = u_light.ambient * texture(u_material.diffuse, texCoords).rgb;

        // Diffuse light
        vec3 norm = normalize(normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = u_light.diffuse * diff * texture(u_material.diffuse, texCoords).rgb;
        diffuse *= attenuation;

        // Specular light
        vec3 viewDir = normalize(u_viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
        vec3 specular = u_light.specular * spec * texture(u_material.specular, texCoords).rgb;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
    else
    {
        FragColor = vec4(u_light.ambient * texture(u_material.diffuse, texCoords).rgb, 1.0);
    }
}