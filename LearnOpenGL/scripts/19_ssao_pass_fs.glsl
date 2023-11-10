#version 330 core

in vec2 ioTexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D uTexNoise;

uniform vec3 uSamples[64];
uniform mat4 uProjectionMatrix;

out float FragColor;

// Global parameters (you'd probably want to use them as uniforms to more easily tweak the effect).
const int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// It's the tile noise texture over screen based on screen dimensions divided by noise size.
const vec2 noiseScale = vec2(1280.0 / 4.0, 720.0 / 4.0);

void main()
{
    // Get inputs for SSAO algorithm.
    vec3 fragPos = texture(gPosition, ioTexCoords).xyz;
    vec3 fragNormal = normalize(texture(gNormal, ioTexCoords).xyz);
    vec3 randomVector = normalize(texture(uTexNoise, ioTexCoords * noiseScale).xyz);

    // Create TBN change-of-basis matrix: from tangent-space to view-space.
    vec3 tangent = normalize(randomVector - fragNormal * dot(randomVector, fragNormal));
    vec3 bitangent = cross(fragNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, fragNormal);

    // Iterate over the sample kernel and calculate occlusion factor.
    float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // Get sample position.
        vec3 samplePos = TBN * uSamples[i]; // From tangent to view-space.
        samplePos = fragPos + samplePos * radius;
        
        // Project sample position (to sample texture) (to get position on screen/texture).
        vec4 offset = vec4(samplePos, 1.0);
        offset = uProjectionMatrix * offset; // From view to clip-space.
        offset.xyz /= offset.w; // Perspective divide.
        offset.xyz = offset.xyz * 0.5 + 0.5; // Transform to range [0.0 - 1.0].
        
        // Get sample depth.
        float sampleDepth = texture(gPosition, offset.xy).z; // Get depth value of kernel sample.
        
        // Calculate the range check & accumulate.
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));

        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}