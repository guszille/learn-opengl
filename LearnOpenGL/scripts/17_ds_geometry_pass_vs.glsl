#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform bool uInversedNormals = false;

out vec3 ioFragPos;
out vec2 ioTexCoords;
out vec3 ioNormal;

void main()
{
    vec4 wPos = uModelMatrix * vec4(aPos, 1.0); // World position.
    vec4 vPos = uViewMatrix * wPos; // View position.
    mat3 normalMatrix = transpose(inverse(mat3(uViewMatrix * uModelMatrix)));

    ioFragPos = vPos.xyz;
    ioTexCoords = aTexCoords;
    ioNormal = normalMatrix * (uInversedNormals ? -aNormal : aNormal);

    gl_Position = uProjectionMatrix * vPos;
}