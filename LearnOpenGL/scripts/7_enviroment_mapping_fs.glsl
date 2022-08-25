#version 330 core

in vec3 oiNormal;
in vec3 oiPosition;

uniform vec3 uViewPos;
uniform samplerCube uCubeMap;
uniform int uMappingStrategy = 0;

out vec4 FragColor;

void main()
{
    vec3 I = normalize(oiPosition - uViewPos);

    switch (uMappingStrategy)
    {
    case 0: // Reflection.
        vec3 R1 = reflect(I, normalize(oiNormal));

        FragColor = vec4(texture(uCubeMap, R1).rgb, 1.0);

        break;

    case 1: // Refraction.
        float ratio = 1.00 / 1.52; // The light ray goes from air to glass.

        // Do note that for physically accurate results we should refract the light
        // again when it leaves the object; now we simply used single-sided refraction
        // which is fine for most purposes.
        //
        vec3 R2 = refract(I, normalize(oiNormal), ratio);

        FragColor = vec4(texture(uCubeMap, R2).rgb, 1.0);

        break;

    default:
        FragColor = vec4(1.0);

        break;
    }
}