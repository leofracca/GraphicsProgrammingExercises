#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_shadowMap;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;

struct DirectionalLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight u_directionalLight;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perform perspective divide (transform the light-space fragment position in range [-1, 1])
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range (because the depth in the depth map is in the range [0, 1])
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective
    float closestDepth = texture(u_shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow (with bias to solve Shadow Acne artifacts)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(u_lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; // 1: shadow, 0: not shadow

    // PCF
    /*int samples = 0;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            samples += 1;

            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= samples;*/

    // Another PCF technique: smoother result
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);
    const int halfkernelWidth = 3;
    for(int x = -halfkernelWidth; x <= halfkernelWidth; ++x)
    {
	    for(int y = -halfkernelWidth; y <= halfkernelWidth; ++y)
	    {
		    float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
		    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
	    }
    }
    shadow /= ((halfkernelWidth*2+1)*(halfkernelWidth*2+1));

    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{           
    vec3 color = texture(u_diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.2 * u_directionalLight.ambient;
    // diffuse
    vec3 lightDir = normalize(u_lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * u_directionalLight.diffuse;
    // specular
    vec3 viewDir = normalize(u_viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * u_directionalLight.specular;  
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}