#version 330 core

out vec4 FragColor;

uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uObjectColor;

in vec3 oiFragPos;
in vec3 oiFragNormal;

void main()
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir   = normalize(uLightPos - oiFragPos);
    vec3 viewDir    = normalize(uViewPos - oiFragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);

    float ambientStrength   = 0.1;
    float diffuseStrength   = max(dot(fragNormal, lightDir), 0.0);
    float specularShininess = 32;
    float specularStrength  = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);

    vec3 ambient  = ambientStrength * uLightColor;
    vec3 diffuse  = diffuseStrength * uLightColor;
    vec3 specular = specularStrength * uLightColor;
    vec3 result   = (ambient + diffuse + specular) * uObjectColor;

    FragColor = vec4(result, 1.0);
}