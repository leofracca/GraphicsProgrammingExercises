#version 330 core

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords; // For the texture of the diffuse

out vec4 FragColor;

/*struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};*/

struct DirectionalLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 u_viewPos;
// uniform Material u_material;
uniform DirectionalLight u_directionalLight;
uniform SpotLight u_spotLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);//u_material.shininess);
    // Combine results
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, texCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);//u_material.shininess);

    // Spotlight properties
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutoff - light.outerCutoff);
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0); // Clamp the intensity between 0 and 1 (0: outside the cone, 1 inside the inner cone, between 0 and 1 outside the inner cone but inside the outer cone)

    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, texCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(vec3(texture(texture_normal1, texCoords)));
    vec3 viewDir = normalize(u_viewPos - fragPos);

    // Directional Light
    vec3 result = CalcDirectionalLight(u_directionalLight, norm, viewDir);

    // Spot Light
    result += CalcSpotLight(u_spotLight, norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}