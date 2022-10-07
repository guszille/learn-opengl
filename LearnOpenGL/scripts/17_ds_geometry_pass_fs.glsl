#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoAndSpecular;

in vec3 ioFragPos;
in vec2 ioTexCoords;
in vec3 ioNormal;

uniform sampler2D uDiffuseMap;
uniform sampler2D uSpecularMap;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture.
    gPosition = ioFragPos;

    // Also store the per-fragment normals into the gbuffer.
    gNormal = normalize(ioNormal);

    // And the diffuse per-fragment color.
    gAlbedoAndSpecular.rgb = texture(uDiffuseMap, ioTexCoords).rgb;

    // Finally, store specular intensity in gAlbedoAndSpecular's alpha component.
    gAlbedoAndSpecular.a = texture(uSpecularMap, ioTexCoords).r;
}