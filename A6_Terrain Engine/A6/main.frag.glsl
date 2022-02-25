#version 410 core

in vec2 TexCoord;

out vec4 color;

uniform float alpha;
uniform sampler2D ourTexture;

void main()
{
    color = texture(ourTexture, TexCoord) * vec4(1.0f, 1.0f, 1.0f, alpha);
}