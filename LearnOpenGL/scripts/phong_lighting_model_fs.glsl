#version 330 core

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 

in vec3 oiFragPos;
in vec3 oiFragNormal;
in vec2 oiTexCoords;

uniform Light uLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

out vec4 FragColor;

void main()
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir = normalize(uLight.position - oiFragPos);
    vec3 viewDir = normalize(uViewPos - oiFragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);

    // Calculating "Phong Lighting" components.

    vec3 ambient = uLight.ambient * vec3(texture(uMaterial.diffuse, oiTexCoords));
    vec3 diffuse = uLight.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuse, oiTexCoords)));
    vec3 specular = uLight.specular * (specularStr * vec3(texture(uMaterial.specular, oiTexCoords)));

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}