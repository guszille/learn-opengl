#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uLightSpaceMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(uModelMatrix))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = uLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.FragPos, 1.0);
}