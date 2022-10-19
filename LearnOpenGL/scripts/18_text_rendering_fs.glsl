#version 330 core

in vec2 ioTexCoords;

out vec4 Color;

uniform sampler2D uText;
uniform vec3 uTextColor;

void main()
{
    Color = vec4(uTextColor, 1.0) * vec4(1.0, 1.0, 1.0, texture(uText, ioTexCoords).r);
}