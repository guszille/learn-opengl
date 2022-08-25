#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 oiNormal;
out vec3 oiPosition;

void main()
{
    oiNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal;
    oiPosition = vec3(uModelMatrix * vec4(aPos, 1.0));

    gl_Position = uProjectionMatrix * uViewMatrix * vec4(oiPosition, 1.0);
}