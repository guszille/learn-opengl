#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform vec3 uViewPos;
uniform vec3 uLightPos;

out ioVS2FS
{
    vec3 FragPos;
    vec3 FragNormal;
    vec2 TexCoords;
    vec3 ViewPosTS;
    vec3 LightPosTS;
    vec3 FragPosTS;
} vsOUT;

void main()
{
    vec3 T = normalize(vec3(uModelMatrix * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(uModelMatrix * vec4(aBitangent, 0.0))); // Same as: vec3 B = cross(N, T);
    vec3 N = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));
    mat3 TBN = transpose(mat3(T, B, N));

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);

    vsOUT.FragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    vsOUT.FragNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal; // Preserving FragNormal to alternate between techniques in the fragment shader.
    vsOUT.TexCoords = aTexCoords;
    vsOUT.ViewPosTS = TBN * uViewPos;
    vsOUT.LightPosTS = TBN * uLightPos;
    vsOUT.FragPosTS = TBN * vsOUT.FragPos;
}