#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix; // a.k.a. model matrix.

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 oiFragPos;
out vec3 oiFragNormal;
out vec2 oiTexCoords;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * aInstanceMatrix * vec4(aPos, 1.0);

    oiFragPos = vec3(aInstanceMatrix * vec4(aPos, 1.0));
    oiFragNormal = mat3(transpose(inverse(aInstanceMatrix))) * aNormal;
    oiTexCoords = aTexCoords;
}