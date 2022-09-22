#version 330 core

struct Light // Represents a point light type.
{
    vec3 ambientComp;
    vec3 diffuseComp;
    vec3 specularComp;

    vec3 position; // WARNING: Is not being used, since light calculations are done in tangent space.
};

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D parallaxMap; // Or depthMap.

    float     shininess;
}; 

in ioVS2FS
{
    vec3 FragPos;
    vec3 FragNormal;
    vec2 TexCoords;
    vec3 ViewPosTS;
    vec3 LightPosTS;
    vec3 FragPosTS;
} fsIN;

uniform Light uLight;
uniform Material uMaterial;
uniform float uHeightScale = 1.0;
uniform bool uEnableNPMapping = false; // To enable/disable normal and parallax mapping.

out vec4 FragColor;

vec2 calcParallaxSimpleMapping(vec3 viewDir)
{
    float height = texture(uMaterial.parallaxMap, fsIN.TexCoords).r;

    return fsIN.TexCoords - viewDir.xy * (height * uHeightScale);
}

vec2 calcParallaxOcclusionMapping(vec3 viewDir)
{
    const float minOfLayers = 8.0;
    const float maxOfLayers = 32.0;

    float numberOfLayers = mix(maxOfLayers, minOfLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
    float layerDepth = 1.0 / numberOfLayers;

    // The amount to shift the texture coordinates per layer (from vector P).
    vec2 P = viewDir.xy * uHeightScale;
    vec2 currentTexCoords = fsIN.TexCoords;
    vec2 deltaTexCoords = P / numberOfLayers;

    float currentLayerDepth = 0.0;
    float currentDepthMapValue = texture(uMaterial.parallaxMap, fsIN.TexCoords).r;
  
    while (currentLayerDepth < currentDepthMapValue)
    {
        // Shift texture coordinates along direction of P.
        currentTexCoords -= deltaTexCoords;

        // Get depthmap value at current texture coordinates.
        currentDepthMapValue = texture(uMaterial.parallaxMap, currentTexCoords).r;

        // Get depth of next layer.
        currentLayerDepth += layerDepth;
    }

    // Get texture coordinates before collision (reverse operations).
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // Get the depth before and after collision for linear interpolation.
    float beforeDepth = texture(uMaterial.parallaxMap, prevTexCoords).r - currentLayerDepth + layerDepth;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);

    // Execute the interpolation of texture coordinates.
    return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}

void main()
{  
    vec3 lightDir = normalize(fsIN.LightPosTS - fsIN.FragPosTS);
    vec3 viewDir = normalize(fsIN.ViewPosTS - fsIN.FragPosTS);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec2 texCoords = calcParallaxOcclusionMapping(viewDir); // Displaced texture coordinates.
    vec3 fragNormal = normalize(texture(uMaterial.normalMap, texCoords).rgb * 2.0 - 1.0);

    if (!uEnableNPMapping) // When normal and parallax mapping are disabled.
    {
        texCoords = fsIN.TexCoords;
        fragNormal = normalize(fsIN.FragNormal);
    }

    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    {
        discard;
    }

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    vec3 ambient = uLight.ambientComp * vec3(texture(uMaterial.diffuseMap, texCoords));
    vec3 diffuse = uLight.diffuseComp * (diffuseStr * vec3(texture(uMaterial.diffuseMap, texCoords)));
    vec3 specular = uLight.specularComp * (specularStr * vec3(texture(uMaterial.specularMap, texCoords)));

    float distance = length(uLight.position - fsIN.FragPos);
    float attenuation = 1.0 / (distance * distance);

    diffuse  *= attenuation;
    specular *= attenuation;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}