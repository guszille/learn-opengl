#version 330 core

in vec2 oiTexCoords;
  
uniform sampler2D uScreenTexture;
uniform int uActiveEffect = 0; // Zero means "no effect".

out vec4 FragColor;

vec4 processKernel(float[9] kernel)
{
    const float offset = 1.0 / 300.0;

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0,     offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0),    // center-left
        vec2( 0.0,     0.0),    // center-center
        vec2( offset,  0.0),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0,    -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    vec3 color = vec3(0.0);

    for(int i = 0; i < 9; i++)
    {
        vec3 sampledTex = vec3(texture(uScreenTexture, oiTexCoords.st + offsets[i]));

        color += sampledTex * kernel[i];
    }

    return vec4(color, 1.0);
}

void main()
{
    vec4 pixel = texture(uScreenTexture, oiTexCoords);

    float sharpenKernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    float blurKernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );

    float edgeDetectionKernel[9] = float[](
        1,  1,  1,
        1, -8,  1,
        1,  1,  1
    );

    // Enabling post-processing effects.
    //
    switch (uActiveEffect)
    {
    case 1: // Inversion.
        FragColor = vec4(vec3(1.0 - pixel), 1.0);
        break;

    case 2: // Grey scale.
        float average = (pixel.r + pixel.g + pixel.b) / 3.0;

        // To get the most physically accurate results
        // we'll need to use weighted channels.
        //
        // float average = 0.2126 * pixel.r + 0.7152 * pixel.g + 0.0722 * pixel.b;

        FragColor = vec4(average, average, average, 1.0);
        break;

    case 3: // Kernel.
        FragColor = processKernel(sharpenKernel);
        break;

    case 4: // Blur.
        FragColor = processKernel(blurKernel);
        break;

    case 5: // Edge-detection.
        FragColor = processKernel(edgeDetectionKernel);
        break;

    default:
        FragColor = pixel;
        break;
    }
}