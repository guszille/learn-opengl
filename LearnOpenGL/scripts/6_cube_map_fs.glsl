#version 330 core

in vec3 ioTexCoord;

uniform samplerCube uCubeMap;

out vec4 FragColor;

void main()
{
	FragColor = texture(uCubeMap, ioTexCoord);
}