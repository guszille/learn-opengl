#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 ioFragPos;
out vec3 ioFragNormal;
out vec2 ioTexCoords;
out mat3 ioTBN;

void main()
{
    // TODO:
    //
    // So instead of sending the inverse of the TBN matrix to the fragment shader, we send a
    // tangent-space light position, view position, and vertex position to the fragment shader.
    // This saves us from having to do matrix multiplications in the fragment shader.

    vec3 T = normalize(vec3(uModelMatrix * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(uModelMatrix * vec4(aBitangent, 0.0))); // vec3 B = cross(N, T);
    vec3 N = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));

    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);

    ioFragPos = vec3(uModelMatrix * vec4(aPos, 1.0));
    ioFragNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal;
    ioTexCoords = aTexCoords;
    ioTBN = mat3(T, B, N);
}