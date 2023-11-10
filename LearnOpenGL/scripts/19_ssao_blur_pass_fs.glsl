#version 330 core

in vec2 ioTexCoords;

uniform sampler2D uSSAORaw;

out float FragColor;

void main()
{
    vec2  texelSize = 1.0 / vec2(textureSize(uSSAORaw, 0));
    float result = 0.0;

    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;

            result += texture(uSSAORaw, ioTexCoords + offset).r;
        }
    }

    FragColor = result / (4.0 * 4.0);
}