#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

uniform mat4 u_lightSpaceMatrix;
uniform mat4 u_model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_finalBonesMatrices[MAX_BONES];

void main()
{
    //gl_Position = u_lightSpaceMatrix * u_model * vec4(inPos, 1.0);

    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(inPos,1.0f);
            break;
        }
        vec4 localPosition = u_finalBonesMatrices[boneIds[i]] * vec4(inPos,1.0f);
        totalPosition += localPosition * weights[i];
   }
	
    gl_Position =  u_lightSpaceMatrix * u_model * totalPosition;
}