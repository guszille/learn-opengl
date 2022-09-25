#version 330 core

#define N_POINT_LIGHTS 4

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

in vec3 oiFragPos;
in vec3 oiFragNormal;
in vec2 oiTexCoords;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLights[N_POINT_LIGHTS];
uniform SpotLight uSpotLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

uniform bool uActivateDirectionalLight = true;
uniform bool uActivatePointLights = true;
uniform bool uActivateSpotLight = true;

out vec4 FragColor;

vec3 calcDirectionalLight(DirectionalLight lightSource)
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir   = normalize(-lightSource.direction);
    vec3 viewDir    = normalize(uViewPos - oiFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr  = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    // Calculating "Blinn-Phong Lighting" components.

    vec3 ambient  = lightSource.ambient * vec3(texture(uMaterial.diffuse, oiTexCoords));
    vec3 diffuse  = lightSource.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuse, oiTexCoords)));
    vec3 specular = lightSource.specular * (specularStr * vec3(texture(uMaterial.specular, oiTexCoords)));

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight lightSource)
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir   = normalize(lightSource.position - oiFragPos);
    vec3 viewDir    = normalize(uViewPos - oiFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr  = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);
    float distance    = length(lightSource.position - oiFragPos);
    float attenuation = 1.0 / (lightSource.constant + lightSource.linear * distance + lightSource.quadratic * (distance * distance));

    // Calculating "Blinn-Phong Lighting" components.

    vec3 ambient  = lightSource.ambient * vec3(texture(uMaterial.diffuse, oiTexCoords));
    vec3 diffuse  = lightSource.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuse, oiTexCoords)));
    vec3 specular = lightSource.specular * (specularStr * vec3(texture(uMaterial.specular, oiTexCoords)));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calcSpotLight(SpotLight lightSource)
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir   = normalize(lightSource.position - oiFragPos);
    vec3 viewDir    = normalize(uViewPos - oiFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float theta     = dot(lightDir, normalize(-lightSource.direction));
    float epsilon   = lightSource.cutOff - lightSource.outerCutOff;
    float intensity = clamp((theta - lightSource.outerCutOff) / epsilon, 0.0, 1.0);
    
    float diffuseStr  = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);
    float distance    = length(lightSource.position - oiFragPos);
    float attenuation = 1.0 / (lightSource.constant + lightSource.linear * distance + lightSource.quadratic * (distance * distance));

    // Calculating "Blinn-Phong Lighting" components.

    vec3 ambient  = lightSource.ambient * vec3(texture(uMaterial.diffuse, oiTexCoords));
    vec3 diffuse  = lightSource.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuse, oiTexCoords)));
    vec3 specular = lightSource.specular * (specularStr * vec3(texture(uMaterial.specular, oiTexCoords)));

    diffuse  *= intensity;
    specular *= intensity;
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular; 
}

void main()
{
    // TODO:
    //
    // There are lot of duplicated calculations in this approach spread out over the light type functions
    // (e.g. calculating the reflect vector, diffuse and specular terms, and sampling the material textures)
    // so there's room for optimization here.
    //
    vec3 result = vec3(0.0);

    // 1. Directional light.
    if (uActivateDirectionalLight)
    {
        result += calcDirectionalLight(uDirectionalLight);
    }

    // 2. Point lights.
    if (uActivatePointLights)
    {
        for(int i = 0; i < N_POINT_LIGHTS; i++)
        {
            result += calcPointLight(uPointLights[i]);
        }
    }

    // 3. Spot light.
    if (uActivateSpotLight)
    {
        result += calcSpotLight(uSpotLight);
    }

    FragColor = vec4(result, 1.0);
}