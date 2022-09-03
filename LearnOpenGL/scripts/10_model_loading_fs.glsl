#version 330 core

#define N_POINT_LIGHTS 4
#define N_MAT_COLOR_MAPS 1

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
    sampler2D diffuseMaps[N_MAT_COLOR_MAPS];
    sampler2D specularMaps[N_MAT_COLOR_MAPS];

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
uniform bool uActivateLights = false;

out vec4 FragColor;

vec3 calcDirectionalLight(DirectionalLight lightSource)
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir   = normalize(-lightSource.direction);
    vec3 viewDir    = normalize(uViewPos - oiFragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);

    float diffuseStr  = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);

    // Calculating "Phong Lighting" components.

    vec3 ambient  = lightSource.ambient * vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords));
    vec3 diffuse  = lightSource.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords)));
    vec3 specular = lightSource.specular * (specularStr * vec3(texture(uMaterial.specularMaps[0], oiTexCoords)));

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight lightSource)
{
    vec3 fragNormal = normalize(oiFragNormal);
    vec3 lightDir   = normalize(lightSource.position - oiFragPos);
    vec3 viewDir    = normalize(uViewPos - oiFragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);

    float diffuseStr  = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    float distance    = length(lightSource.position - oiFragPos);
    float attenuation = 1.0 / (lightSource.constant + lightSource.linear * distance + lightSource.quadratic * (distance * distance));

    // Calculating "Phong Lighting" components.

    vec3 ambient  = lightSource.ambient * vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords));
    vec3 diffuse  = lightSource.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords)));
    vec3 specular = lightSource.specular * (specularStr * vec3(texture(uMaterial.specularMaps[0], oiTexCoords)));

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
    vec3 reflectDir = reflect(-lightDir, fragNormal);

    float theta     = dot(lightDir, normalize(-lightSource.direction));
    float epsilon   = lightSource.cutOff - lightSource.outerCutOff;
    float intensity = clamp((theta - lightSource.outerCutOff) / epsilon, 0.0, 1.0);
    
    float diffuseStr  = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = 0.5 * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    float distance    = length(lightSource.position - oiFragPos);
    float attenuation = 1.0 / (lightSource.constant + lightSource.linear * distance + lightSource.quadratic * (distance * distance));

    // Calculating "Phong Lighting" components.

    vec3 ambient  = lightSource.ambient * vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords));
    vec3 diffuse  = lightSource.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords)));
    vec3 specular = lightSource.specular * (specularStr * vec3(texture(uMaterial.specularMaps[0], oiTexCoords)));

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

    if(uActivateLights)
    {
        // WARNING:
        //
        // We are using only the first diffuse and specular maps in light calculations,
        // at the position 0 (zero).

        // 1. Directional light.
        result += calcDirectionalLight(uDirectionalLight);

        // 2. Point lights. (DEACTIVATED)
        //
        // for(int i = 0; i < N_POINT_LIGHTS; i++)
        // {
        //     result += calcPointLight(uPointLights[i]);
        // }

        // 3. Spot light.
        result += calcSpotLight(uSpotLight);
    }
    else
    {
        result = vec3(texture(uMaterial.diffuseMaps[0], oiTexCoords));
    }

    FragColor = vec4(result, 1.0);
}