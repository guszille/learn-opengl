#version 330 core

out vec4 FragColor;

in vec2 ioTexCoord;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;

void main()
{
	FragColor = mix(texture(uTexture0, ioTexCoord), texture(uTexture1, ioTexCoord), 0.2);
}