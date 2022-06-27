#version 330 core

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
}; 

out vec4 FragColor;

uniform Light uLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

in vec3 oiFragPos;
in vec3 oiFragNormal;

void main()
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir = normalize(uLight.position - oiFragPos);
    vec3 viewDir = normalize(uViewPos - oiFragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);

    float diffuseStrength = max(dot(fragNormal, lightDir), 0.0);
    float specularStrength = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);

    // Calculating "Phong Lighting" components.

    vec3 ambient = uLight.ambient * uMaterial.ambient;
    vec3 diffuse = uLight.diffuse * (diffuseStrength * uMaterial.diffuse);
    vec3 specular = uLight.specular * (specularStrength * uMaterial.specular);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}