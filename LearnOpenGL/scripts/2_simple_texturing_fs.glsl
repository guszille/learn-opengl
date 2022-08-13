#version 330 core

in vec2 ioTexCoord;

uniform sampler2D uTexture;
uniform float uAlphaThreshold = 0.0;

out vec4 FragColor;

void main()
{
	vec4 textureColor = texture(uTexture, ioTexCoord);

	if (textureColor.a < uAlphaThreshold)
	{
		discard;
	}

	FragColor = textureColor;
}