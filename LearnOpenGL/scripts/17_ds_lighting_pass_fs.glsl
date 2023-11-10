#version 330 core

#define N_POINT_LIGHTS 1

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
uniform sampler2D uSSAO;

uniform Light uLights[N_POINT_LIGHTS];
uniform bool uActivateLighting = true;
uniform bool uProcessAllLightSources = true;
uniform vec3 uViewPos;

out vec4 FragColor;

vec3 calcPointLight(Light lightSource, float lightDis, vec3 fragPos, vec3 fragNormal, vec3 fragDiffuseComp, float fragSpecularComp)
{
    vec3 lightDir = normalize(lightSource.position - fragPos);
    vec3 viewDir = normalize(uViewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fragNormal, halfwayDir), 0.0), 8.0);
    float attenuation = 1.0 / (lightSource.constant + lightSource.linear * lightDis + lightSource.quadratic * (lightDis * lightDis));

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
    vec3 ambientComp = vec3(0.3 * fragDiffuseAndSpecular.rgb * texture(uSSAO, ioTexCoords).r);

    pixelColor += ambientComp;

    if (uActivateLighting)
    {
        for (int i = 0; i < N_POINT_LIGHTS; i++)
        {
            float distance = length(uLights[i].position - fragPos);

            if(uProcessAllLightSources || distance < uLights[i].radius)
            {
                pixelColor += calcPointLight(uLights[i], distance, fragPos, fragNormal, fragDiffuseAndSpecular.rgb, fragDiffuseAndSpecular.a);
            }
        }
    }

    FragColor = vec4(pixelColor, 1.0);
}