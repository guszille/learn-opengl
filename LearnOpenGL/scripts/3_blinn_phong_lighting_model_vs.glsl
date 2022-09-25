#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform bool uInverseNormals = false;

out vec3 oiFragPos;
out vec3 oiFragNormal;
out vec2 oiTexCoords;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);

    oiFragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    oiFragNormal = mat3(transpose(inverse(uModelMatrix))) * (uInverseNormals ? (-1.0 * aNormal) : aNormal);
    oiTexCoords = aTexCoords;
}