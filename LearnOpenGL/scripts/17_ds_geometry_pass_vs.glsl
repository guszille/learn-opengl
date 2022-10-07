#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 ioFragPos;
out vec2 ioTexCoords;
out vec3 ioNormal;

void main()
{
    vec4 wPos = uModelMatrix * vec4(aPos, 1.0); // World position.
    mat3 normalMatrix = transpose(inverse(mat3(uModelMatrix)));

    ioFragPos = wPos.xyz;
    ioTexCoords = aTexCoords;
    
    ioNormal = normalMatrix * aNormal;

    gl_Position = uProjectionMatrix * uViewMatrix * wPos;
}