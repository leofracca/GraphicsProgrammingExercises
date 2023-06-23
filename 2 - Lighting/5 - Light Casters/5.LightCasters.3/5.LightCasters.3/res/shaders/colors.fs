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
    float outerCutoff;

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
    
    // Spotlight (soft edges)
    float theta = dot(lightDir, normalize(-u_light.direction)); 
    float epsilon = (u_light.cutoff - u_light.outerCutoff);
    float intensity = clamp((theta - u_light.outerCutoff) / epsilon, 0.0, 1.0); // Clamp the intensity between 0 and 1 (0: outside the cone, 1 inside the inner cone, between 0 and 1 outside the inner cone but inside the outer cone)

    // Attenuation
    float distance    = length(u_light.position - fragPos);
    float attenuation = 1.0 / (u_light.constant + u_light.linear * distance + u_light.quadratic * (distance * distance));

    // Ambient light
    vec3 ambient = u_light.ambient * texture(u_material.diffuse, texCoords).rgb;
    ambient *= attenuation;

    // Diffuse light
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_light.diffuse * diff * texture(u_material.diffuse, texCoords).rgb;
    diffuse *= attenuation;
    diffuse  *= intensity;

    // Specular light
    vec3 viewDir = normalize(u_viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    vec3 specular = u_light.specular * spec * texture(u_material.specular, texCoords).rgb;
    specular *= attenuation;
    specular *= intensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}