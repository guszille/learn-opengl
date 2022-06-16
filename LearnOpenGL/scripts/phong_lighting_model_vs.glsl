#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 oiFragPos;
out vec3 oiFragNormal;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);

    oiFragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    oiFragNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal;
}