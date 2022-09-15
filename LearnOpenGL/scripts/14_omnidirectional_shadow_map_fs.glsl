#version 330 core

in vec4 ioFragPos;

uniform vec3 uLightPosition;
uniform float uFarPlane;

void main()
{
    // Get distance between fragment and light source.
    float lightDistance = length(ioFragPos.xyz - uLightPosition);
    
    // Map to [0;1] range by dividing by "far plane".
    lightDistance = lightDistance / uFarPlane;
    
    // Write this as modified depth.
    gl_FragDepth = lightDistance;
}