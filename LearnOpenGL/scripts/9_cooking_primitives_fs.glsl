#version 330 core

in GS_OUT
{
    vec2 ioTexCoords;
} fs_in;

uniform sampler2D uTexture;

out vec4 FragColor;

void main()
{
    FragColor = texture(uTexture, fs_in.ioTexCoords);
}