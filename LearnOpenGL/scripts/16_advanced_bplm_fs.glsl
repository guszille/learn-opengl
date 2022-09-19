#version 330 core

struct Light // Represents a point light type.
{
    vec3 ambientComp;
    vec3 diffuseComp;
    vec3 specularComp;

    vec3 position;
};

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;

    float     shininess;
}; 

in vec3 ioFragPos;
in vec3 ioFragNormal;
in vec2 ioTexCoords;
in mat3 ioTBN;

uniform Light uLight;
uniform Material uMaterial;
uniform vec3 uViewPos;
uniform bool uDisableNormalMapping = false;

out vec4 FragColor;

void main()
{
    vec3 fragNormal = normalize(ioTBN * (texture(uMaterial.normalMap, ioTexCoords).rgb * 2.0 - 1.0)); // Obtain the normal vector in range [0,1] and transform it to range [-1,1].
    
    if (uDisableNormalMapping) { fragNormal = normalize(ioFragNormal); } // Enabled by default.
    
    vec3 lightDir = normalize(uLight.position - ioFragPos);
    vec3 viewDir = normalize(uViewPos - ioFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    // Calculating "Blinn-Phong Lighting" components.

    vec3 ambient = uLight.ambientComp * vec3(texture(uMaterial.diffuseMap, ioTexCoords));
    vec3 diffuse = uLight.diffuseComp * (diffuseStr * vec3(texture(uMaterial.diffuseMap, ioTexCoords)));
    vec3 specular = uLight.specularComp * (specularStr * vec3(texture(uMaterial.specularMap, ioTexCoords)));

    // Applying distance attenuation.

    float distance = length(uLight.position - ioFragPos);
    float attenuation = 1.0 / (distance * distance);

    diffuse  *= attenuation;
    specular *= attenuation;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}