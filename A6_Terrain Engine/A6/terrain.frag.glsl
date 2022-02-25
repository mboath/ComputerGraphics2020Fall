#version 410 core

in vec2 TexCoord1;
in vec2 TexCoord2;
in float FragY;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
    vec4 color1 = texture(ourTexture1, TexCoord1);
    vec4 color2 = texture(ourTexture2, TexCoord2);
    float alpha;
    if (FragY < 0.0f)
        alpha = 0.0f;
    else
        alpha = 1.0f;
    vec4 ourcolor = vec4(color1.x+color2.x-0.5f, color1.y+color2.y-0.5, color1.z+color2.z-0.5f, alpha);
    if (ourcolor.a == 0.0f)
        discard;
    color = ourcolor;
}