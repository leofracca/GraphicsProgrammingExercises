#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_depthMap;
uniform float u_near_plane;
uniform float u_far_plane;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_near_plane * u_far_plane) / (u_far_plane + u_near_plane - z * (u_far_plane - u_near_plane));	
}

void main()
{             
    float depthValue = texture(u_depthMap, TexCoords).r;
    //FragColor = vec4(vec3(LinearizeDepth(depthValue) / u_far_plane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}