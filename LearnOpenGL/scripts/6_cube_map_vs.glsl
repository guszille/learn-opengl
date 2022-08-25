#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 ioTexCoord;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform bool uTrickDepthBuffer = false;

void main()
{
    // To trick the depth buffer into believing that the skybox has the maximum
    // depth value of 1.0 so that it fails the depth test wherever there's a different
    // object in front of it.
    //
    if (uTrickDepthBuffer)
    {
        vec4 p = uProjectionMatrix * uViewMatrix * vec4(aPos, 1.0);

        gl_Position = p.xyww;
    }
    else
    {
        gl_Position = uProjectionMatrix * uViewMatrix * vec4(aPos, 1.0);
    }

    ioTexCoord = aPos;
}