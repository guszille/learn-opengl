#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

uniform float uPointSize = 0.0; // Requires enabling OpenGL's GL_PROGRAM_POINT_SIZE.
uniform bool uEnablePointSize = false;

void main()
{
    gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);

    if (uEnablePointSize)
    {
        gl_PointSize = uPointSize;
    }
}