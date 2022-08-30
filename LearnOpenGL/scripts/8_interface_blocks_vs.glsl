#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModelMatrix;

layout (std140) uniform uMatrices
{
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
};

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}