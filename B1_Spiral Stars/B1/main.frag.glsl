#version 410 core

in vec2 TexCoord;

out vec4 color;

uniform vec3 starColor;
uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    float r = sqrt(pow(TexCoord.x - 0.5f, 2.0f) + pow(TexCoord.y - 0.5f, 2.0f));
    texColor = texColor * vec4(starColor, max(0.0f, 1.0f - r / 0.3f));
    color = texColor;
}