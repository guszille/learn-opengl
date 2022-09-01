#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 uModelMatrix;

layout (std140) uniform uMatrices
{
    mat4 uViewMatrix;
    mat4 uProjectionMatrix;
};

out VS_OUT
{
    vec2 ioTexCoords;
} vs_out;

void main()
{
    vs_out.ioTexCoords = aTexCoords;

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}