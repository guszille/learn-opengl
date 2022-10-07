#version 330 core

#define N_POINT_LIGHTS 32

struct Light // Represents a point light type.
{
    vec3 position;
    vec3 color;
	
    float constant;
    float linear;
    float quadratic;

    float radius;
};

in vec2 ioTexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoAndSpecular;

uniform Light uLights[N_POINT_LIGHTS];
uniform bool uActivateLighting = true;
uniform vec3 uViewPos;

out vec4 FragColor;

vec3 calcPointLight(Light lightSource, vec3 fragPos, vec3 fragNormal, vec3 fragDiffuseComp, float fragSpecularComp)
{
    vec3 lightDir = normalize(lightSource.position - fragPos);
    vec3 viewDir = normalize(uViewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(fragNormal, halfwayDir), 0.0), 16.0);
    float distance = length(lightSource.position - fragPos);
    float attenuation = 1.0 / (lightSource.constant + lightSource.linear * distance + lightSource.quadratic * (distance * distance));

    // Calculating the rest of the "Blinn-Phong Lighting" components.

    vec3 diffuse = lightSource.color * (diffuseStr * fragDiffuseComp);
    vec3 specular = lightSource.color * (specularStr * fragSpecularComp);

    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

void main()
{
    vec3 pixelColor = vec3(0.0);

    vec3 fragPos = texture(gPosition, ioTexCoords).rgb;
    vec3 fragNormal = texture(gNormal, ioTexCoords).rgb;
    vec4 fragDiffuseAndSpecular = texture(gAlbedoAndSpecular, ioTexCoords).rgba;
    vec3 ambientComp = 0.1 * fragDiffuseAndSpecular.rgb; // Hard-coded ambient component.

    pixelColor += ambientComp;

    if (uActivateLighting)
    {
        for (int i = 0; i < N_POINT_LIGHTS; i++)
        {
            float distance = length(uLights[i].position - fragPos);

            if(distance < uLights[i].radius)
            {
                pixelColor += calcPointLight(uLights[i], fragPos, fragNormal, fragDiffuseAndSpecular.rgb, fragDiffuseAndSpecular.a);
            }
        }
    }

    FragColor = vec4(pixelColor, 1.0);
}