#version 410 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float index;

uniform mat4 view;
uniform mat4 projection;
uniform float x;
uniform float z;
uniform vec4 height;
uniform vec2 offset;
uniform float scale;
uniform int reflection;

out vec2 TexCoord1;
out vec2 TexCoord2;
out float FragY;

void main()
{
    int i = int(index);
    if (reflection == 0)
        gl_Position = projection * view * vec4(position.x + x, height[i], position.y + z, 1.0f);
    else
        gl_Position = projection * view * vec4(position.x + x, -height[i], position.y + z, 1.0f);
    TexCoord1 = vec2(texCoord.x + offset.x, 1.0f - texCoord.y - offset.y);
    TexCoord2 = vec2(TexCoord1.x * scale, TexCoord1.y * scale);
    FragY = height[i];
}
