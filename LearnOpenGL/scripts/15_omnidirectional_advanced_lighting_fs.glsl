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

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform Light uLight;
uniform Material uMaterial;
uniform samplerCube uShadowMap;
uniform vec3 uViewPos;
uniform float uFarPlane;

out vec4 FragColor;

float calcShadowingFactor(vec3 lightDir)
{
    vec3 sampleOffsetDirections[20] = vec3[]
    (
        vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
        vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
        vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
        vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
    );

    // Get vector between fragment position and light position.
    vec3 fragToLight = fs_in.FragPos - uLight.position; 

    // Use the light to fragment vector to sample from the depth map.
    // float closestDepth = texture(uShadowMap, fragToLight).r;

    // It is currently in linear range between [0,1], so re-transform back to original value.
    // closestDepth *= uFarPlane;

    // Now get current linear depth as the length between the fragment and light position.
    float currentDepth = length(fragToLight);

    // Finally, test for shadows.
    int samples = 20;

    float bias = 0.15;
    float factor = 0.0;
    float viewDistance = length(uViewPos - fs_in.FragPos);
    float diskRadius = (1.0 + (viewDistance / uFarPlane)) / 25.0;

    // Applying percentage-closer filtering.
    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(uShadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;

        closestDepth *= uFarPlane; // Undo mapping [0;1].

        if (currentDepth - bias > closestDepth)
        {
            factor += 1.0;
        }
    }

    factor /= float(samples);

    return factor;
}

void main()
{
    vec3 fragNormal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(uLight.position - fs_in.FragPos);
    vec3 viewDir = normalize(uViewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diffuseStr = max(dot(fragNormal, lightDir), 0.0);
    float specularStr = pow(max(dot(fragNormal, halfwayDir), 0.0), uMaterial.shininess);

    // Calculating "Blinn-Phong Lighting" components.

    vec3 ambient = uLight.ambient * vec3(texture(uMaterial.diffuse, fs_in.TexCoords));
    vec3 diffuse = uLight.diffuse * (diffuseStr * vec3(texture(uMaterial.diffuse, fs_in.TexCoords)));
    vec3 specular = uLight.specular * (specularStr * vec3(texture(uMaterial.specular, fs_in.TexCoords)));

    // Claculating shadowing.

    float shadowingFactor = calcShadowingFactor(lightDir);

    FragColor = vec4(ambient + ((1.0 - shadowingFactor) * (diffuse + specular)), 1.0);
}