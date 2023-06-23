#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;

void main()
{    
    vs_out.FragPos = vec3(u_model * vec4(inPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(u_model))) * inNormal;
    vs_out.TexCoords = inTexCoords;
    vs_out.FragPosLightSpace = u_lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = u_projection * u_view * vec4(vs_out.FragPos, 1.0);
}